delimiter //
create trigger reduce_half
after update on uosoffering
for each row
begin
if new.Enrollment < new.MaxEnrollment / 2 then 
	begin
		set @message = 'The enrollment is smaller than half.';
	end;
end if;
end //
delimiter ;