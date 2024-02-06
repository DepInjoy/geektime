`DorisFE`是Doris的FE入口，这里开启main函数。

```java
public class DorisFE {
	public static void main(String[] args) {
        StartupOptions options = new StartupOptions();
        options.enableHttpServer = true;
        options.enableQeService = true;
        start(DORIS_HOME_DIR, PID_DIR, args, options);
    }

    public static void start(String dorisHomeDir, String pidDir,
                             String[] args, StartupOptions options) {
    CommandLineOptions cmdLineOpts = parseArgs(args);
    try {
        // 1. 创建和锁定pid文件
        if (!createAndLockPidFile(pidDir + "/fe.pid")) {
            throw new IOException("pid file is already locked.");
        }

        // 2. init config
        Config config = new Config();
        config.init(dorisHomeDir + "/conf/fe.conf");
        config.initCustom(Config.custom_config_dir + "/fe_custom.conf");
        LOCK_FILE_PATH = Config.meta_dir + "/" + LOCK_FILE_NAME;
        tryLockProcess();
        LdapConfig ldapConfig = new LdapConfig();
        if (new File(dorisHomeDir + "/conf/ldap.conf").exists()) {
            ldapConfig.init(dorisHomeDir + "/conf/ldap.conf");
        }

        // 3. check java version
        if (!JdkUtils.checkJavaVersion()) {
            throw new IllegalArgumentException("Java version doesn't match");
        }
       	
        Log4jConfig.initLogging(dorisHomeDir + "/conf/");
        Runtime.getRuntime().addShutdownHook(new Thread(LogManager::shutdown));

        // 4. set dns cache ttl
        java.security.Security.setProperty("networkaddress.cache.ttl", "60");

        // 5. check command line options
        checkCommandLineOptions(cmdLineOpts);

        LOG.info("Doris FE starting...");
        
        // 6. init Fe Option,端口检查
        FrontendOptions.init();
        checkAllPorts();
        System.setProperty("software.amazon.awssdk.http.service.impl",
                "software.amazon.awssdk.http.urlconnection.UrlConnectionSdkHttpService");

        // 7. init catalog and wait it be ready
        Env.getCurrentEnv().initialize(args);
        Env.getCurrentEnv().waitForReady();
        Telemetry.initOpenTelemetry();

        // 8. init and start server
        // 	8.1. HttpServer for HTTP Server
        // 	8.2. FeServer for Thrift Server
        // 	8.3. QeService for MySQL Server
        FeServer feServer = new FeServer(Config.rpc_port);
        feServer.start();
        if (options.enableHttpServer) {
            HttpServer httpServer = new HttpServer();
            // .... set参数
            httpServer.start();
            Env.getCurrentEnv().setHttpReady(true);
        }

        if (options.enableQeService) { // MysqlService
            QeService qeService = new QeService(Config.query_port, 
                        ExecuteEnv.getInstance().getScheduler());
            qeService.start();
        }
        ThreadPoolManager.registerAllThreadPoolMetric();
        while (true) hread.sleep(2000);
    } catch (Throwable e) {
        e.printStackTrace();
        LOG.warn("", e);
    }
}
```

## MySQL  Server

其中，`org.apache.doris.qe.QeService`对应的是MySQL Server

```java
public class QeService {
    private int port;
    private MysqlServer mysqlServer; // MySQL protocol server
    public QeService(int port, ConnectScheduler scheduler) {
        this.port = port;
        this.mysqlServer = new MysqlServer(port, scheduler);
    }

    public void start() throws Exception {
        // Set up help module
        HelpModule.getInstance().setUpModule(HelpModule.HELP_ZIP_FILE_NAME);

        if (!mysqlServer.start()) {
            LOG.error("mysql server start failed");
            System.exit(-1);
        }
    }
}
```

