# SSDB
### Simple simple DB
## Phase 1
supports list
- two operation (insert / select)
- store only in memory (no persistence to disk)
- support single hard-code table 
## Table

For first phase the table structure is hard-code and its single one.

| Column | Type |
| ------ | ------ |
| id | integer |
| username | varchar(32) |
| email | varchar(255) |

## Set up the DB and some command sample

1. Run the bin file in out/bin/SSDB by ./out/bin/SSDB mydb.db
1. Exit the prompt with !exit
1. run command (:


```
ssdb > insert 1 cstack foo@bar.com
Executed.
ssdb > insert 2 bob bob@example.com
Executed.
ssdb > select
(1, cstack, foo@bar.com)
(2, bob, bob@example.com)
Executed.
ssdb > insert foo bar 1
Syntax error. Could not parse statement.
ssdb > .exit
```

## TDD of DB
for making test i use RSpec you can test it with following command

```plaintext
$ bundle config set --local path '.bundle'
$ bundle install
$ bundle exec rspec
```



### Reference
Until phase 1 i complete the db with [this](https://cstack.github.io/db_tutorial/) great tutorial
