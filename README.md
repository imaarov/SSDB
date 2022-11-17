# SSDB
### Simple simple DB
## phase 1
supports list
- two operation (insert / select)
- store only in memory (no persistence to disk)
- support single hard-code table 
## Table

For first phase the table structure are hard-code and its single one.

| Column | Type |
| ------ | ------ |
| id | integer |
| username | varchar(32) |
| email | varchar(255) |

## Set up the DB and some command sample

1. Run the bin file in out/bin/SSDB by ./SSDB
1. Exit the prompt with !exit
1. run command (:


```
db > insert 1 cstack foo@bar.com
Executed.
db > insert 2 bob bob@example.com
Executed.
db > select
(1, cstack, foo@bar.com)
(2, bob, bob@example.com)
Executed.
db > insert foo bar 1
Syntax error. Could not parse statement.
db > .exit
