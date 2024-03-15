
多源数据目录（Multi-Catalog）功能，旨在能够更方便对接外部数据目录，以增强doris的数据湖分析和联邦数据查询能力。Catalog可以直接对应到外部数据目录，解决`create external table`和`create external database`需要手动进行一一映射的问题，构成Catalog -> Database -> Table的三层元数据层级
- InternalCatalog: doris原有的db和table归属于InternalCatalog
- ExternalCatalog: 外部数据源的catalog，支持
    - Apache Hive
    - Apache Iceberg
    - Apache Hudi
    - Elasticsearch
    - Apache Paimon
    - JDBC Catalog，通过标准JDBC协议连接数据源，Doris会自动同步数据源下的Database和Table元数据，以便快速访问这些外部数据，支持 MySQL、PostgreSQL、Oracle、SQLServer、Clickhouse、Doris、SAP HANA、Trino/Presto、OceanBase等。
    - 等等


# 前端
```plantuml
@startuml
interface JdbcClient {
    + static JdbcClient createJdbcClient(\n\tJdbcClientConfig jdbcClientConfig)

    # Type jdbcTypeToDoris(JdbcFieldSchema fieldSchema)
    + List<Column> getColumnsFromJdbc(\n\tString dbName, String tableName)
}

class JdbcExternalTable {
    - JdbcTable jdbcTable

    + List<Column> initSchema()
    + JdbcTable getJdbcTable()
}

class JdbcScanNode {
    - JdbcTable tbl
    # void toThrift(TPlanNode msg)
}
note top: 转化为TJdbcScanNode供后端执行

abstract class ExternalScanNode {
    << 统一从外部catalog读取数据接口 >>
    # boolean needCheckColumnPriv
    # final FederationBackendPolicy backendPolicy
}

abstract class ScanNode {
    # TupleDescriptor desc
    # List<TScanRangeLocations> scanRangeLocations
    # PartitionInfo partitionsInfo
}

abstract class PlanNode {

}

JdbcPostgreSQLClient -up-|> JdbcClient
JdbcExternalTable <-- JdbcClient : 用getColumnsFromJdbc来initSchema
JdbcExternalTable -left-> JdbcScanNode : 填充JdbcTable信息
JdbcScanNode -down-|> ExternalScanNode
ExternalScanNode -down-|> ScanNode
ScanNode -down-|> PlanNode

JdbcExternalTable -right-|> ExternalTable
@enduml
```

# 执行算子
```plantuml
@startuml
class NewJdbcScanner {
    - std::unique_ptr<JdbcConnector> _jdbc_connector
    - const TupleDescriptor* _tuple_desc
    - JdbcConnectorParam _jdbc_param
}
NewJdbcScanner -right-|> VScanner
JdbcConnector -up-o NewJdbcScanner
@enduml
```