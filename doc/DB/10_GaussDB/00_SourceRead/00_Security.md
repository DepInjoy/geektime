# 透明加密TDE
```sql
ALTER TABLE <name> ENCRYPTION KEY ROTATION
```
```c
#define PageIsTDE(page) (((PageHeader)(page))->pd_flags & PD_TDE_PAGE)
#define PageSetTDE(page) (((PageHeader)(page))->pd_flags |= PD_TDE_PAGE)
#define PageClearTDE(page) (((PageHeader)(page))->pd_flags &= ~PD_TDE_PAGE
```
```c
typedef struct {
    /* The maximum length is used to meet AES256 and AES128/SM4 */
    uint8 dek_cipher[DEK_CIPHER_AES256_LEN_PAGE];
    uint8 cmk_id[CMK_ID_LEN_PAGE];
    uint8 iv[RANDOM_IV_LEN];
    uint8 tag[GCM_TAG_LEN];
    uint8 algo;
    uint8 res[RES_LEN];
} TdePageInfo;
```
```c
// Block data encrypt
char* PageDataEncryptIfNeed(Page page, TdeInfo* tde_info, bool need_copy, bool is_segbuf)
```
---

`raw_heap_insert`
```c
static void raw_heap_insert(RewriteState state, HeapTuple tup)
{
    Page page = state->rs_buffer;
    Size page_free_space, save_free_space;
    Size len;
    OffsetNumber newoff;
    HeapTuple heaptup;
    TransactionId xmin, xmax;

    if (state->rs_new_rel->rd_rel->relkind == RELKIND_TOASTVALUE) {
        /* toast table entries should never be recursively toasted */
        heaptup = tup;
    } else if (HeapTupleHasExternal(tup) || tup->t_len > TOAST_TUPLE_THRESHOLD)
        heaptup = toast_insert_or_update(state->rs_new_rel, tup, NULL,
            HEAP_INSERT_SKIP_FSM | (state->rs_use_wal ? 0 : HEAP_INSERT_SKIP_WAL), NULL);
    else
        heaptup = tup;

    len = MAXALIGN(heaptup->t_len); /* be conservative */
    /* Compute desired extra freespace due to fillfactor option */
    save_free_space = RelationGetTargetPageFreeSpace(
            state->rs_new_rel, HEAP_DEFAULT_FILLFACTOR);

    // 2. 检查是否有足够的空闲空间
    if (state->rs_buffer_valid) {
        page_free_space = PageGetHeapFreeSpace(page);
        if (len + save_free_space > page_free_space) {
            // 2.1 有足够的空间，这里和TDE相关有两个主要接口
            //  2.1.1 GetTdeInfoFromRel从输入的Relation中构建出TdeInfo
            //  2.1.2 PageDataEncryptIfNeed : Block data encrypt
            rewrite_write_one_page(state, page);
            state->rs_blockno++;
            state->rs_buffer_valid = false;
        }
    }

    // 2.2 空闲空间不足，新建Page
    if (!state->rs_buffer_valid) {
        HeapPageHeader phdr = (HeapPageHeader)page;
        // 2.2.1 初始化一个新的空Page
        PageInit(page, BLCKSZ, 0, true);
        phdr->pd_xid_base = u_sess->utils_cxt.RecentXmin - FirstNormalTransactionId;
        phdr->pd_multi_base = 0;
        state->rs_buffer_valid = true;
        const char* algo = RelationGetAlgo(state->rs_new_rel);
        if (RelationisEncryptEnable(state->rs_new_rel) || (algo && *algo != '\0')) {
            // 2.2.2 开启TDE，
            phdr->pd_upper -= sizeof(TdePageInfo);
            phdr->pd_special -= sizeof(TdePageInfo);
            PageSetTDE(page);
        }
    }

    xmin = HeapTupleGetRawXmin(heaptup);
    xmax = HeapTupleGetRawXmax(heaptup);
    rewrite_page_prepare_for_xid(page, xmin, false);
    (void)rewrite_page_prepare_for_xid(page, xmax,
        (heaptup->t_data->t_infomask & HEAP_XMAX_IS_MULTI) ? true : false);

    HeapTupleCopyBaseFromPage(heaptup, page);
    HeapTupleSetXmin(heaptup, xmin);
    HeapTupleSetXmax(heaptup, xmax);

    /* And now we can insert the tuple into the page */
    newoff = PageAddItem(page, (Item)heaptup->t_data,
        heaptup->t_len, InvalidOffsetNumber, false, true);

    /* Update caller's t_self to the actual position where it was stored */
    ItemPointerSet(&(tup->t_self), state->rs_blockno, newoff);

    /*
     * Insert the correct position into CTID of the stored tuple, too, if the
     * caller didn't supply a valid CTID.
     */
    if (!ItemPointerIsValid(&tup->t_data->t_ctid)) {
        ItemId newitemid;
        HeapTupleHeader onpage_tup;
        newitemid = PageGetItemId(page, newoff);
        onpage_tup = (HeapTupleHeader)PageGetItem(page, newitemid);
        onpage_tup->t_ctid = tup->t_self;
    }

    /* If heaptup is a private copy, release it. */
    if (heaptup != tup)
        heap_freetuple(heaptup);
}
```

## Block Data Encrypt
```c
char* PageDataEncryptIfNeed(Page page, TdeInfo* tde_info, bool need_copy, bool is_segbuf) {
    size_t plainLength = 0;
    size_t cipherLength = 0;
    errno_t ret = 0;
    int retval = 0;
    TdePageInfo tde_page_info;
    char* dst = NULL;

    // 判断是否需要TDE加密
    if (PageIsNew(page) || !PageIsTDE(page) || !g_instance.attr.attr_security.enable_tde) {
        return (char*)page;
    }

    plainLength = ((PageHeader)page)->pd_special - ((PageHeader)page)->pd_upper;
    // 1. 生成随机iv初始化向量
    retval = RAND_priv_bytes(tde_info->iv, RANDOM_IV_LEN);

    if (need_copy) {
        AllocPageCopyMem();
        dst = is_segbuf ? t_thrd.storage_cxt.segPageCopy : t_thrd.storage_cxt.pageCopy;
        ret = memcpy_s(dst, BLCKSZ, (char*)page, BLCKSZ);
        securec_check(ret, "\0", "\0");
    } else {
        dst = (char*)page;
    }

    // 2. 对Block或CU数据加密，主逻辑是encrypt_partial_mode
    // 
    encryptBlockOrCUData(dst + ((PageHeader)dst)->pd_upper,
        plainLength, dst + ((PageHeader)dst)->pd_upper,
        &cipherLength, tde_info);

    // 3. 将TdeInfo转换为TdePageInfo并写入
    ret = memset_s(&tde_page_info, sizeof(TdePageInfo), 0, sizeof(TdePageInfo));
    securec_check(ret, "\0", "\0");
    // TdeInfo转换为TdePageInfo主要是将16进制转换为int减少空间占用
    transformTdeInfoToPage(tde_info, &tde_page_info);
    // BLCKSZ是数据块(Block)或Page的大小
    ret = memcpy_s(dst + BLCKSZ - sizeof(TdePageInfo), sizeof(TdePageInfo),
        &tde_page_info, sizeof(TdePageInfo));
    securec_check(ret, "\0", "\0");

    // 4. 设置页加密标志, 即((((PageHeader)(page))->pd_flags |= PD_ENCRYPT_PAGE)
    PageSetEncrypt((Page)dst);
    return dst;
}
```

### 加密计算
```c
// 对Block或CU除Header和填充数据进行加密
void encryptBlockOrCUData(const char* plainText, const size_t plainLength,
    char* cipherText, size_t* cipherLength, TdeInfo* tdeInfo) {
    int retryCnt = 3; 
    unsigned char* iv = tdeInfo->iv;
    TdeAlgo algo = (TdeAlgo)tdeInfo->algo;
    unsigned char key[KEY_128BIT_LEN] = {0};
    const char* plain_key = NULL;
    unsigned int i, j;
    errno_t ret = 0;

    // 1. 根据cmk_id和dek_cipher获取数据密钥的明文
    TDEKeyManager *tde_key_manager = New(CurrentMemoryContext) TDEKeyManager();
    tde_key_manager->init();
    plain_key = tde_key_manager->get_key(tdeInfo->cmk_id, tdeInfo->dek_cipher);

    // 将数据密钥明文16进制转换为int
    for (i = 0, j = 0; i < KEY_128BIT_LEN && j < strlen(plain_key); i++, j += 2) {
        key[i] = hex2int(plain_key[j]) << 4 | hex2int(plain_key[j + 1]);
    }
    DELETE_EX2(tde_key_manager);

    // 2. 执行加密运算，如果失败，最多尝试3次
    do {
        if (encrypt_partial_mode(plainText, plainLength, 
                cipherText, cipherLength, key, iv, algo)) {
            break;
        }
        retryCnt--;
    } while (retryCnt > 0);
    ret = memset_s(key, KEY_128BIT_LEN, 0, KEY_128BIT_LEN);
    securec_check(ret, "\0", "\0");

    if (retryCnt == 0) ereport(PANIC, ......);
}
```

`encrypt_partial_mode`执行加密运算，忽略一些出错处理逻辑来了解其主要处理逻辑。
```c
// 支持AES128/SM4算法，采用CTR计算模式
bool encrypt_partial_mode(const char* plainText, const size_t plainLength, char* cipherText,
    size_t* cipherLength, unsigned char* key, unsigned char* iv, TdeAlgo algo) {
    const char* algoText = NULL;
    ENGINE* engine = NULL;
    // 1. 创建EVP对象指针
    const EVP_CIPHER* cipher = NULL;
    switch (algo) {
        case TDE_ALGO_AES_128_CTR:
            algoText = "aes-128-ctr";
            cipher = EVP_aes_128_ctr();
            break;
        case TDE_ALGO_SM4_CTR:
            algoText = "sm4-ctr";
            engine = init_cipher_engine();
            cipher = EVP_sm4_ctr();
            break;
        default:
            break;
    }

    // 2. 开始数据加密
    bool result = ctr_enc_partial_mode(algoText, engine, cipher, plainText,
            plainLength, cipherText, cipherLength, key, iv);
    return result;
}

bool ctr_enc_partial_mode(const char* encalgoText, ENGINE* engine, const EVP_CIPHER* cipher,
    const char* plainText, const size_t plainLength, char* cipherText, size_t* cipherLength,
    unsigned char* key, unsigned char* iv) {
    int SegCipherLength = 0;
    int LastSegCipherLength = 0;
    int ret = 0;

    EVP_CIPHER_CTX* ctx = NULL;
    // 1.创建并始化上下文
    ctx = EVP_CIPHER_CTX_new();

    // 2. 初始化加密操作
    ret = EVP_EncryptInit_ex(ctx, cipher, engine, key, iv);

    // 3. 加密数据
    ret = EVP_EncryptUpdate(ctx, (unsigned char*)cipherText,
        &SegCipherLength, (unsigned char*)plainText, plainLength);

    // 4. 完成加密操作
    ret = EVP_EncryptFinal_ex(ctx, (unsigned char*)cipherText +
        SegCipherLength, &LastSegCipherLength);
    
    *cipherLength = SegCipherLength + LastSegCipherLength;
    // 5. 释放上下文
    EVP_CIPHER_CTX_free(ctx);
    return true;
}
```
##
```c
void PageDataDecryptIfNeed(Page page) {
    TdeInfo tde_info = {0};
    TdePageInfo* tde_page_info = NULL;

    if (PageIsEncrypt(page) && PageIsTDE(page)) {
        size_t plainLength = 0;
        size_t cipherLength = ((PageHeader)page)->pd_special - ((PageHeader)page)->pd_upper;
        tde_page_info = (TdePageInfo*)((char*)(page) + BLCKSZ - sizeof(TdePageInfo));
        // 1. 从Page中获取TdeInfo
        transformTdeInfoFromPage(&tde_info, tde_page_info);

        // 2. 对Block或CU数据解密，主逻辑是decrypt_partial_mode
        decryptBlockOrCUData(page + ((PageHeader)page)->pd_upper,
            cipherLength,
            page + ((PageHeader)page)->pd_upper,
            &plainLength,  &tde_info);

        /* clear the encryption flag */
        PageClearEncrypt(page);
    }
}
```

### 解密计算
对Block或CU除Header和填充数据进行解密运算。
```c
// src/gausskernel/cbb/utils/aes/cipherfn.cpp
void decryptBlockOrCUData(const char* cipherText, const size_t cipherLength,
    char* plainText, size_t* plainLength, TdeInfo* tdeInfo) {
    int retryCnt = 3;
    unsigned char* iv = tdeInfo->iv;
    TdeAlgo algo = (TdeAlgo)tdeInfo->algo;
    unsigned char key[KEY_128BIT_LEN] = {0};
    const char* plain_key = NULL;
    unsigned int i, j;
    errno_t ret = 0;

    // 1. 根据cmk_id和dek_cipher获取数据密钥的明文
    TDEKeyManager *tde_key_manager = New(CurrentMemoryContext) TDEKeyManager();
    tde_key_manager->init();
    plain_key = tde_key_manager->get_key(tdeInfo->cmk_id, tdeInfo->dek_cipher);

    // 将数据密钥明文16进制转换为int
    for (i = 0, j = 0; i < KEY_128BIT_LEN && j < strlen(plain_key); i++, j += 2) {
        key[i] = hex2int(plain_key[j]) << 4 | hex2int(plain_key[j + 1]);
    }
    DELETE_EX2(tde_key_manager);

    // 2. 执行解密运算，如果失败，最多尝试3次
    do {
        if (decrypt_partial_mode(cipherText, cipherLength,
                plainText, plainLength, key, iv, algo)) {
            break;
        }
        retryCnt--;
    } while (retryCnt > 0);

    ret = memset_s(key, KEY_128BIT_LEN, 0, KEY_128BIT_LEN);
    securec_check(ret, "\0", "\0");

    if (retryCnt == 0) {
        ereport(PANIC, .....);
    }
}

bool decrypt_partial_mode(const char* cipherText, const size_t cipherLength,
    char* plainText, size_t* plainLength, unsigned char* key,
    unsigned char* iv, TdeAlgo algo) {
    const char* algoText = NULL;
    ENGINE* engine = NULL;
    const EVP_CIPHER* cipher = NULL;
    // 1. 创建EVP对象指针
    switch (algo) {
        case TDE_ALGO_AES_128_CTR:
            algoText = "aes-128-ctr";
            cipher = EVP_aes_128_ctr();
            break;
        case TDE_ALGO_SM4_CTR:
            algoText = "sm4-ctr";
            engine = init_cipher_engine();
            cipher = EVP_sm4_ctr();
            break;
        default:
            break;
    }

    // 2. 开始数据解密
    bool result = ctr_dec_partial_mode(algoText, engine, cipher, cipherText,
        cipherLength, plainText, plainLength, key, iv);
    return result;
}

bool ctr_dec_partial_mode(const char* decalgoText, ENGINE* engine, const EVP_CIPHER* cipher,   
    const char* cipherText, const size_t cipherLength, char* plainText,
    size_t* plainLength, unsigned char* key, unsigned char* iv) {
    int segPlainLength = 0;
    int lastSegPlainLength = 0;
    int ret = 0;

    // 1.创建并始化上下文
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();

    // 2. 初始化解密操作
    ret = EVP_DecryptInit_ex(ctx, cipher, engine, key, iv);

    // 3. 解密数据
    ret = EVP_DecryptUpdate(ctx, (unsigned char*)plainText, &segPlainLength,
            (unsigned char*)cipherText, cipherLength);

    // 4. 完成解密操作
    ret = EVP_DecryptFinal_ex(ctx, (unsigned char*)plainText + segPlainLength,
            &lastSegPlainLength);
    
    *plainLength = segPlainLength + lastSegPlainLength;
    // 5. 释放
    EVP_CIPHER_CTX_free(ctx);
    return true;
}
```

---

`GetTdeInfoFromRel`从输入的`Relation`中构建出`TdeInfo`.
```C++
// src/common/backend/utils/cache/relcache.cpp
void GetTdeInfoFromRel(Relation rel, TdeInfo *tde_info)
{
    errno_t rc = 0;
    rc = memset_s(tde_info, sizeof(TdeInfo), 0, sizeof(TdeInfo));
    securec_check(rc, "\0", "\0");

    const char* dek_cipher = RelationGetDekCipher(rel);
    const char* cmk_id = RelationGetCmkId(rel);
    const char* algo = RelationGetAlgo(rel);

    rc = strcpy_s(tde_info->dek_cipher, DEK_CIPHER_LEN, dek_cipher);
    securec_check(rc, "\0", "\0");
    rc = strcpy_s(tde_info->cmk_id, CMK_ID_LEN, cmk_id);
    securec_check(rc, "\0", "\0");
    if (pg_strcasecmp((char*)algo, "AES_128_CTR") == 0) {
        tde_info->algo = (uint8)TDE_ALGO_AES_128_CTR;
    } else if (pg_strcasecmp((char*)algo, "SM4_CTR") == 0) {
        tde_info->algo = (uint8)TDE_ALGO_SM4_CTR;
    } else {
        tde_info->algo = (uint8)TDE_ALGO_NONE;
    }
}
```

```c
// src/include/utils/rel.h
typedef struct RelationData* Relation;
typedef struct RelationData {
                ......
    RelFileNode rd_node; /* relation physical identifier */
    bytea* rd_options; /* parsed pg_class.reloptions */
                ......
};

typedef struct StdRdOptions {
            ......
    char* dek_cipher;
    char* cmk_id;
    char* encrypt_algo;
    bool enable_tde;     /* switch flag for table-level TDE encryption */
            ......
} StdRdOptions;

#define RelationisEncryptEnable(relation) \
        (((relation)->rd_options && (relation)->rd_rel->relkind == RELKIND_RELATION) ? \
        ((StdRdOptions *)(relation)->rd_options)->enable_tde : false)

#define RelationGetDekCipher(relation) \
        StdRdOptionsGetStringData((relation)->rd_options, dek_cipher, NULL)

#define RelationGetCmkId(relation) \
        StdRdOptionsGetStringData((relation)->rd_options, cmk_id, NULL)

#define RelationGetAlgo(relation) \
        StdRdOptionsGetStringData((relation)->rd_options, encrypt_algo, NULL)
```

```c
// src/include/tde_key_management/data_common.h

// TdeInfo is used for encryption and decryption
typedef struct {
    char dek_cipher[DEK_CIPHER_LEN];
    char cmk_id[CMK_ID_LEN];
    uint8 iv[RANDOM_IV_LEN];
    uint8 tag[GCM_TAG_LEN];
    uint8 algo;
    uint8 res[RES_LEN];
} TdeInfo;
```




# 全密态
```C++
// src/common/interfaces/libpq/client_logic_hooks/cmk_entity_manager_hooks/reg_hook_frame.h
// 
typedef struct CmkEntityManager {
    CreateCmkObjectHookFunc crt_cmko_hookfunc;
    EncryptCekPlainHookFunc enc_cek_cipher_hookfunc;
    DecryptCekCipherHookFunc dec_cek_plain_hookfunc;
    DropCmKObjectHookFunc drop_cmko_hookfunc;
    PostCreateCmkObjectHookFunc post_crt_cmko_hookfunc;
} CmkEntityManager;
```
```C++
// src/common/interfaces/libpq/client_logic_hooks/cmk_entity_manager_hooks/reg_hook_frame.cpp
CmkemErrCode create_cmk_obj(CmkIdentity *cmk_identity)
CmkemErrCode encrypt_cek_plain(CmkemUStr *cek_plain, CmkIdentity *cmk_identity, CmkemUStr **cek_cipher)
CmkemErrCode encrypt_cek_plain(CmkemUStr *cek_plain, CmkIdentity *cmk_identity, CmkemUStr **cek_cipher)
CmkemErrCode drop_cmk_obj(CmkIdentity *cmk_identity) 
```
```
EncryptionGlobalHookExecutor::pre_create(
    create_cmk_obj

bool EncryptionGlobalHookExecutor::set_deletion_expected()
  ret = drop_cmk_obj(&cmk_identify);
```

```c++
bool CacheLoader::load_client_logic_cache(CacheRefreshType cache_refresh_type, PGconn *conn)

class CacheLoader {

}
// bool CacheLoader::fill_global_settings_map(PGconn *conn)
class CachedGlobalSetting : public CachedSetting {
public:
    CachedGlobalSetting(const Oid oid, const char *database_name, const char *schema_name, const char *object_name);
    ~CachedGlobalSetting();
    void set_global_hook_executor(GlobalHookExecutor *global_hook_executor);
    GlobalHookExecutor *get_executor() const;
    GlobalHookExecutor *m_global_hook_executor;
};

// 
class CachedSetting {
public:
    CachedSetting(const Oid oid, const char *database_name, const char *schema_name, const char *object_name);
protected:
    Oid m_oid;
    NameData m_database_name;
    NameData m_schema_name;
    NameData m_object_name;
    char m_fqdn[NAMEDATALEN * 4];
}

// bool CacheLoader::fill_column_settings_info_cache(PGconn *conn)
class CachedColumnSetting : public CachedSetting {
public:
    CachedColumnSetting(const Oid oid, const char *database_name, const char *schema_name, const char *object_name);
    ~CachedColumnSetting();
    ColumnHookExecutor *get_executor() const;
    void set_executor(ColumnHookExecutor *executor);
    char m_cached_global_setting[4 * NAMEDATALEN + 1];
    ColumnHookExecutor *m_column_hook_executor;
};
```
```C++
class AbstractHookExecutor {
public:
    explicit AbstractHookExecutor(const char *function_name);
    bool add_argument(const char *key, const char *value);
    bool get_argument(const char *key, const char **value, size_t &valueSize) const;
    virtual ~AbstractHookExecutor() = 0;
    virtual void inc_ref_count();
    virtual void dec_ref_count();
    const bool safe_to_remove() const;

protected:
    char **m_allowed_values;
    size_t m_allowed_values_size;
    CStringsMap m_values_map;
    virtual void save_private_variables() = 0;
    static const int m_FUNCTION_NAME_MAX_SIZE = 256;
    void add_allowed_value(const char * const);
    size_t ref_count;

public:
    char m_function_name[m_FUNCTION_NAME_MAX_SIZE];
};
```
```C++
class GlobalHookExecutor : public AbstractHookExecutor {
public:
    virtual bool process(ColumnHookExecutor *column_hook_executor) = 0;
    virtual bool pre_create(const StringArgs &args, const GlobalHookExecutor **existing_global_hook_executors,
        size_t existing_global_hook_executors_size);
    virtual bool post_create(const StringArgs& args);
    virtual bool deprocess_column_setting(const unsigned char *processed_data, size_t processed_data_size, 
    const char *key_store, const char *key_path, const char *key_algo, unsigned char **data, size_t *data_size);

    virtual bool set_deletion_expected();
}
```

```C++
class EncryptionGlobalHookExecutor : public GlobalHookExecutor, public AbstractEncryptionHook {}

class EncryptionGlobalHookExecutor : public GlobalHookExecutor, public AbstractEncryptionHook {}

```

```C++
class GlobalSettings {
  static GlobalHookExecutor *get(const char *function_name, PGClientLogic &);
  static void del(GlobalHookExecutor *global_hook_executor);
}

class ColumnSettings {
public:
    static ColumnHookExecutor *get(const char *function_name, Oid oid, GlobalHookExecutor *global_hook_executor);
    static void del(ColumnHookExecutor *column_hook_executor);
}
```

```C++
class HooksFactory {
private:
    static const int m_FUNCTION_NAME_SIZE = 256;

public:
    class GlobalSettings {
    public:
        static GlobalHookExecutor *get(const char *function_name, PGClientLogic &);
        static void del(GlobalHookExecutor *global_hook_executor);

    private:
        /* CREATE */
        typedef GlobalHookExecutor *(*GlobalHookExecutorCreator)(PGClientLogic &client_logic);
        typedef struct GlobalHooksCreatorFuncsMapping {
            char name[m_FUNCTION_NAME_SIZE];
            GlobalHookExecutorCreator executor;
        } GlobalHooksCreatorFuncsMapping;
        static GlobalHooksCreatorFuncsMapping m_global_hooks_creator_mapping[HOOKS_NUM];

        /* DELETE */
        typedef void (*GlobalHookExecutorDeleter)(GlobalHookExecutor *);
        typedef struct GlobalHooksDeleterFunctionsMapping {
            char name[m_FUNCTION_NAME_SIZE];
            GlobalHookExecutorDeleter executor;
        } GlobalHooksDeleterFunctionsMapping;
        static GlobalHooksDeleterFunctionsMapping m_global_hooks_deleter_mapping[HOOKS_NUM];
    };

    class ColumnSettings {
    public:
        static ColumnHookExecutor *get(const char *function_name, Oid oid, GlobalHookExecutor *global_hook_executor);
        static void del(ColumnHookExecutor *column_hook_executor);

    private:
        typedef ColumnHookExecutor *(*ColumnHookExecutorCreator)(GlobalHookExecutor *, Oid);
        typedef struct ColumnHooksCreatorFunctionsMapping {
            char name[m_FUNCTION_NAME_SIZE];
            ColumnHookExecutorCreator executor;
        } ColumnHooksCreatorFunctionsMapping;
        static ColumnHooksCreatorFunctionsMapping m_column_hooks_creator_mapping[HOOKS_NUM];

        typedef void (*ColumnHookExecutorDeleter)(ColumnHookExecutor *);
        typedef struct ColumnHooksDeleterFunctionsMapping {
            char name[m_FUNCTION_NAME_SIZE];
            ColumnHookExecutorDeleter executor;
        } ColumnHooksDeleterFunctionsMapping;
        static ColumnHooksDeleterFunctionsMapping m_column_hooks_deleter_mapping[HOOKS_NUM];
    };
};
```