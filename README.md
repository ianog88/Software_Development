# Software Development
This respository contains code files intended to illustrate my understanding of object orientated programming and software development. Both files make use of core programming concepts such as variables, conditional and looping statements, functions and classes as well as more advanced concepts such as inheritance, wrapper classes and threading. This corresponds to the learning outcomes of the modules
- Software Development 1 [SWDV H1001](https://www.tudublin.ie/study/modules/swdv-h1001-software-development-1/)
- Software Development 2 [SWDV H1002](https://www.tudublin.ie/study/modules/swdv-h1002-software-development-2/)
- Software Development 3 [SWDV H2001](https://www.tudublin.ie/study/modules/swdv-h2001-software-development-3/)
- Software Development 4 [SWDV H2002](https://www.tudublin.ie/study/modules/swdv-h2002-software-development-4/)

## Strategy Backtest ([backtest_4.py](backtest_4.py))
This file contains the source code for one of my early backtests. It works by returning key metrics of a sample strategy using historical data from interactive brokers API. 

## Strategy Implementation ([strat_4.py](strat_4.py))
This file contains the source code for the implementation of a trading strategy I created. It works by connecting to interactive brokers servers via a websocket following the TCP protocol. I chose the TCP protocol over the HTTP protocol due to the streaming nature of the price data.  
