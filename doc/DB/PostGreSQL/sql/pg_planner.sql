-- 搭建测试环境并创建测试数据，开始
create table student(sno int, sname varchar(16), ssex varchar(1));
create table course(cno int, cname varchar(16), tno int);
create table score(sno int, cno varchar(16), degree int);
create table teacher(tno int, tname varchar(16), tsex varchar(1));

insert into student values(5, 'zs', 'M');
insert into student values(3, 'ls', 'F');
insert into student values(2, 'ww', 'M');
insert into student values(4, 'zl', 'M');
insert into student values(1, 'lq', 'F');

insert into course values(1, 'English', 2);
insert into course values(2, 'Math', 5);
insert into course values(3, 'Data', 3);
insert into course values(4, 'Design', 5);
insert into course values(5, 'Phys', 6);

insert into score values(2, 1, 60);
insert into score values(3, 2, 50);
insert into score values(1, 3, 80);
insert into score values(1, 5, 90);
insert into score values(4, 4, 85);
insert into score values(3, 3, 99);
insert into score values(5, 1, 78);

insert into teacher values(1, 'Jim', 'M');
insert into teacher values(2, 'Tom', 'F');
insert into teacher values(3, 'Lucy', 'M');
insert into teacher values(4, 'Dadge', 'F');
insert into teacher values(5, 'Benny', 'M');
-- 搭建测试环境并创建测试数据，开始

-- 通用表达式
explain analyse
	with cte as (select * from student)
		select sname from cte;
		
explain analyse
	with cte as (select * from student)
		select sname from cte;

explain
with RECURSIVE INC(VAL) as(
	select 1 union all
		select inc.val+1 from inc where inc.val < 10
) select * from inc;

-- 提升子查询
explain
select * from student where exists (
	select sno from score where score.sno=student.sno);

explain
select * from student where exists (select sno from score);

explain
	select * from student where sno > any(select sno from score)