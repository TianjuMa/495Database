DELIMITER //
CREATE PROCEDURE withdraw_procedure (
IN withdraw_course varchar(20),
in student_id varchar(20))
BEGIN
	
    declare se varchar(20);
    declare ye int;
    declare t_error int default 0;
	declare continue handler for sqlexception set t_error = 1;
 
    start transaction;
	set @message = '13';
    select Semester into se from transcript where StudId = student_id and UoSCode = withdraw_course;
    select Year into ye from transcript where StudId = student_id and UoSCode = withdraw_course;
	/*Delete the withdraw_course student entered from his transcript*/
	DELETE FROM transcript
    WHERE UoSCode = withdraw_course and StudId = student_id;
	/* update the Enrollment in UoSoffering by -1*/
	UPDATE uosoffering
    SET uosoffering.Enrollment = uosoffering.Enrollment - 1
    WHERE uosoffering.UoScode = withdraw_course and Year = ye and Semester = se;
     if t_error = 1 then begin
	 set @message = 'rollback';
     rollback;
     end;
     else commit;
     end if;
END //
DELIMITER ;