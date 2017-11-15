DELIMITER //

CREATE PROCEDURE canenroll(
 IN username varchar(20),
 IN cyear int(11),
 IN quarter varchar(20),
 IN courseID varchar(20),
 IN quarter1 varchar(20),
 IN year1 varchar(20),
 IN quarter2 varchar(20),
 IN year2 varchar(20))
Begin

SELECT a.UoSCode
From uosoffering a
where     a.year=cyear
	  and a.semester=quarter
      and a.MaxEnrollment>a.Enrollment 
      and a.Uoscode = courseID
      and ((a.UoSCode in (select b.UoSCode
							from requires b
							where b.PrereqUoSCode in
											(select c.UoSCode
											 from transcript c
											 where c.StudId=username 
											 	and c.grade is not NULL 
											 	and c.grade !="F"
											 	and c.year <= cyear))) or (a.UoSCode not in (select d.Uoscode from requires d)))
	  and a.UoSCode not in ( select t.UoSCode   
                              from  requires t
                              where t.PrereqUoSCode in(select i.UoSCode from transcript i where i.studid = username and i.grade is null))
	  and a.UoSCode not in (select p.UoSCode
	  						from transcript p
	  						where a.UoSCode = p.UoSCode and p.grade != "F")
	  and a.UoSCode in (select yrd.UoSCode from(SELECT aq.UoSCode as UoSCode,aq.Semester as semester,aq.Year as year
                                               FROM uosoffering aq 
   											   where ((aq.Semester = quarter1 and aq.year = year1)
		 											or (aq.Semester = quarter2 and aq.year = year2))
											   and aq.UoSCode not in (select c.UoScode from transcript c where c.StudId = username )) yrd 
	  									   where cyear = yrd.year and quarter = yrd.semester);              
END//	

DELIMITER ;

DELIMITER //

CREATE PROCEDURE cannotenroll
(IN username varchar(20),
 IN cyear int,
 IN quarter char(20),
 IN courseID varchar(20),
 IN quarter1 varchar(20),
 IN year1 varchar(20),
 IN quarter2 varchar(20),
 IN year2 varchar(20))
Begin

SELECT  a.UoSCode
From uosoffering a
where     a.year=cyear
	  and a.semester=quarter
	  and a.Uoscode = courseID
      and a.MaxEnrollment>a.Enrollment 
      and (a.UoSCode  in    (select b.UoSCode
							from requires b
							where b.PrereqUoSCode  in
											(select t.UoSCode
											 from transcript t
											 where t.StudId=username and t.grade is null)))
      	  and a.UoSCode in (select yrd.UoSCode from(SELECT aq.UoSCode as UoSCode,aq.Semester as semester,aq.Year as year
                                               FROM uosoffering aq 
   											   where ((aq.Semester = quarter1 and aq.year = year1)
		 											or (aq.Semester = quarter2 and aq.year = year2))
											   and aq.UoSCode not in (select c.UoScode from transcript c where c.StudId = username )) yrd 
	  									   where cyear = yrd.year and quarter = yrd.semester); 
END//	

DELIMITER ;

DELIMITER //

CREATE PROCEDURE enrolled
(IN username varchar(20),
 IN courseID varchar(20))
Begin

SELECT  a.UoSCode
From uosoffering a
where 	((a.UoSCode in     ( select t.UoSCode   
                           from  transcript t
                           where t.StudId=username and t.grade != "F"))
or  (a.UoSCode in     ( select t.UoSCode   
                           from  transcript t
                           where t.StudId=username and t.grade is null)))
and a.UoSCode = courseID;
END//	

DELIMITER ;

DELIMITER //

CREATE PROCEDURE addcourse
( IN username varchar(20),
  IN courseID varchar(20),
  IN cyear int,
  IN quarter varchar(20))

BEGIN


START TRANSACTION;

Insert INTO transcript
values(username,courseID,quarter,cyear,null);
update uosoffering
set uosoffering.Enrollment=uosoffering.Enrollment+1
where uosoffering.Uoscode=courseID and uosoffering.Semester=quarter and uosoffering.Year=cyear;

COMMIT;
END//	

DELIMITER ;

DELIMITER //

CREATE PROCEDURE prereq
(IN courseID varchar(20),
 In username varchar(20))
Begin

SELECT a.PrereqUoSCode
FROM requires a
WHERE a.UoSCode=courseID 
	and  a.PrereqUoSCode not in (select b.UoSCode 
								 from transcript b 
								 where b.UoSCode = a.PrereqUoSCode 
							     and b.grade is not null 
								 and b.StudId = username);
END//	

DELIMITER ;


