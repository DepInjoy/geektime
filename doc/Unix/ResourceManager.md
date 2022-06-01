# 内存

# 磁盘
磁盘存储
- JBOD(Just Bundle Of Disks), 每一块磁盘没有冗余机制
- RAID(Redundant Array Of Inexpensive Disks), 每一块磁盘在物理上都有冗余机制.

Hadoop的磁盘存储是基于JBOD, 每一块磁盘没有冗余机制, 通过应用层的多副本来保证数据可靠性。Greenplum建议的磁盘存储方式是RAID, 每一块儿磁盘都有冗余机制, 主备方式则是在磁盘冗余基础上提供另一级的数据可靠性。