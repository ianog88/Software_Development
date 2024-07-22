# Software Development
This respository contains code files intended to illustrate my understanding of object orientated programming and software development. I have included two code files I wrote when I was actively trading. Both files make use of core programming concepts such as variables, conditional and looping statements, functions and classes as well as more advanced concepts such as inheritance, wrapper classes and threading. This corresponds to the learning outcomes of the modules
- [Software Development 1 (SWDV H1001)](https://www.tudublin.ie/study/modules/swdv-h1001-software-development-1/)
- [Software Development 2 (SWDV H1002)](https://www.tudublin.ie/study/modules/swdv-h1002-software-development-2/)
- [Software Development 3 (SWDV H2001)](https://www.tudublin.ie/study/modules/swdv-h2001-software-development-3/)
- [Software Development 4 (SWDV H2002)](https://www.tudublin.ie/study/modules/swdv-h2002-software-development-4/)
- [Systems Analysis (SYAN H1004)](https://www.tudublin.ie/study/modules/syan-h1004-systems-analysis/)
- [Systems Analysis & Design (SYAD H2000)](https://www.tudublin.ie/study/modules/syad-h2000-systems-analysis--design/)
- [Applied Software Systems Analysis and Design (OOSD H3002)](https://www.tudublin.ie/study/modules/oosd-h3002-applied-software-systems-analysis-and-design/)
- [Software Quality Assurance and Testing (SOEN H2001)](https://www.tudublin.ie/study/modules/soen-h2001-software-quality-assurance-and-testing/)

## Strategy Implementation ([strat_4.py](strat_4.py))
This file contains the source code for the implementation of a trading strategy I created. The basic structure it follows involves first connecting to Interactive Broker’s servers via TCP connection (chosen over http due to the streaming nature of the requests). It receives current price data using the IBapi.EWrapper interface and passes the data through the main function which decides what actions to take (buy/sell, quantity) based on the result of various predefined functions. Orders are then sent to the brokers servers using the EClientSocket class to be executed. 

### Latency Issues 
This type of software poses a challenge from a latency perspective as it is essentially trading 500 stocks simultaneously. A tick to trade latency greater than 3/4 seconds can significantly diminish the results and is the main reason I no longer run this system. I make use of threading, daemon threads in particular, in an attempt to speed the program up. However, the design of pythons memory management, the global interpreter lock in particular which limits the execution to one thread at a time, meant the latency was still significantly too high. This problem was my motivation for learning C++ which is the current industry standard for trading systems. 

## Strategy Backtest ([backtest_4.py](backtest_4.py))
The purpose of this code is to backtest a trading strategy and return various key performance indicators such as return, sharpe ratio and max drawdown. This involves receiving historical price data, in this scenario I used Interactive Brokers data via the TCP connection however a HTTP protocol would also work fine. I regularly use the yahoo finance api which uses a HTTP protocol in which I store the data in a SQL database. This strategy is not profitable and was never traded live, I’m just using it as an example of code I have written.

## Systems Analysis and Design 
I follow the agile development approach as outlined in the book “Clean Code” which outlines various conventions for writing readable and robust code. These include meaningful names, functions with minimal abstraction and small classes. For version control, I create virtual environments using the anaconda distribution. I conduct unit tests on each function defined in my program and would also run each strategy on a demo account prior to live trading.

