# EOSIO blockchain lab for technologies and freedom 
## tic tac toe smart contract

### commands

#### Create game
```
cleos push action tictactoe create '{"challenger":"challenger", "host":"host"}' --permission host@active
```
#### Make first mode (host)
```
cleos push action tictactoe move '{"challenger":"challenger", "host":"host", "by":"host", "row":0, "column":1}' --permission host@active
```

#### challenger move
```
cleos push action tictactoe move '{"challenger":"challenger", "host":"host", "by":"challenger", "row":1, "column":1}' --permission challenger@active
```
#### view table status
```
cleos get table tictactoe host games
```
#### restart game
```
cleos push action tictactoe restart '{"challenger":"challenger", "host":"host", "by":"host"}' --permission host@active
```
#### close game
```
cleos push action tictactoe close '{"challenger":"challenger", "host":"host"}' --permission host@active
```