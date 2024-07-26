#include "EWrapper.h"
#include "EClientSocket.h"
#include "Contract.h"
#include "Order.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <thread>
#include <mutex>

class IBClient : public EWrapper {
public:
    IBClient();
    ~IBClient();

    void nextValidId(OrderId orderId) override;
    void error(int id, int errorCode, const std::string& errorString) override;
    void tickPrice(TickerId tickerId, TickType field, double price, const TickAttrib& attribs) override;
    void tickSize(TickerId tickerId, TickType field, int size) override;
    void tickString(TickerId tickerId, TickType tickType, const std::string& value) override;
    void tickGeneric(TickerId tickerId, TickType tickType, double value) override;
    void tickEFP(TickerId tickerId, TickType tickType, double basisPoints, const std::string& formattedBasisPoints, double impliedFuture, int holdDays, const std::string& futureExpiry, double dividendImpact, double dividendsToExpiry) override;
    void historicalData(TickerId reqId, const Bar& bar) override;

    void connect(const std::string& host, int port, int clientId);
    void disconnect();
    void requestMarketData(const std::vector<std::string>& symbols);
    void requestHistoricalData(const std::vector<std::string>& symbols);
    void placeOrder(const Contract& contract, const Order& order);
    void findTopDecreasingStocks();

private:
    EClientSocket* client;
    OrderId nextOrderId;
    std::vector<std::pair<std::string, double>> stockChanges;
    std::mutex dataMutex;
};

IBClient::IBClient() : client(new EClientSocket(this)) {}

IBClient::~IBClient() {
    delete client;
}

void IBClient::connect(const std::string& host, int port, int clientId) {
    client->eConnect(host.c_str(), port, clientId);
}

void IBClient::disconnect() {
    client->eDisconnect();
}

void IBClient::nextValidId(OrderId orderId) {
    nextOrderId = orderId;
}

void IBClient::error(int id, int errorCode, const std::string& errorString) {
    std::cerr << "Error: " << errorString << " (code: " << errorCode << ", id: " << id << ")" << std::endl;
}

void IBClient::historicalData(TickerId reqId, const Bar& bar) {
    double change = (bar.close - bar.open) / bar.open * 100;
    std::lock_guard<std::mutex> lock(dataMutex); 
    stockChanges.emplace_back(symbol, change); 
}

void IBClient::requestMarketData(const std::vector<std::string>& symbols) {
    for (const auto& symbol : symbols) {
        Contract contract;
        contract.symbol = symbol;
        contract.secType = "STK";
        contract.exchange = "SMART";
        contract.currency = "USD";

        client->reqMktData(tickerId, contract, "", false, false, TagValueListSPtr());
        std::this_thread::sleep_for(std::chrono::milliseconds(100));  
    }
}

void IBClient::requestHistoricalData(const std::vector<std::string>& symbols) {
    for (const auto& symbol : symbols) {
        Contract contract;
        contract.symbol = symbol;
        contract.secType = "STK";
        contract.exchange = "SMART";
        contract.currency = "USD";

        std::string endDateTime = "";  
        std::string durationStr = "1 D";
        std::string barSizeSetting = "1 day";
        std::string whatToShow = "TRADES";
        int useRTH = 1;
        int formatDate = 1;

        client->reqHistoricalData(tickerId, contract, endDateTime, durationStr, barSizeSetting, whatToShow, useRTH, formatDate, false, TagValueListSPtr());
        std::this_thread::sleep_for(std::chrono::milliseconds(100));  
    }
}

void IBClient::findTopDecreasingStocks() {
    std::lock_guard<std::mutex> lock(dataMutex);
    std::sort(stockChanges.begin(), stockChanges.end(), [](const auto& lhs, const auto& rhs) {
        return lhs.second < rhs.second;
    });

    auto top10 = std::vector<std::pair<std::string, double>>(stockChanges.begin(), stockChanges.begin() + 10);

    for (const auto& stock : top10) {
        std::cout << "Stock: " << stock.first << " Change: " << stock.second << "%" << std::endl;
    }

    for (const auto& stock : top10) {
        Contract contract;
        contract.symbol = stock.first;
        contract.secType = "STK";
        contract.exchange = "SMART";
        contract.currency = "USD";

        Order order;
        order.action = "BUY";
        order.orderType = "MKT";
        order.totalQuantity = 10;  

        placeOrder(contract, order);
    }
}

void IBClient::placeOrder(const Contract& contract, const Order& order) {
    client->placeOrder(nextOrderId++, contract, order);
}

int main() {
    IBClient client;
    client.connect("127.0.0.1", 0000, 00);

    std::vector<std::string> sp500Stocks = 
    {'MMM', 'AOS', 'ABT', 'ABBV', 'ABMD', 'ACN', 'ATVI', 'ADM', 'ADBE', 'ADP', 'AAP', 'AES', 'AFL', 'A', 'AIG', 'APD',
    'AKAM', 'ALK', 'ALB', 'ARE', 'ALGN', 'ALLE', 'LNT', 'ALL', 'GOOGL', 'GOOG', 'MO', 'AMZN', 'AMCR', 'AMD', 'AEE','AAL',
    'AEP', 'AXP', 'AMT', 'AWK', 'AMP', 'ABC', 'AME', 'AMGN', 'APH', 'ADI', 'ANSS', 'ANTM', 'AON', 'APA', 'AAPL','AMAT',
    'APTV', 'ANET', 'AIZ', 'T', 'ATO', 'ADSK', 'AZO', 'AVB', 'AVY', 'BKR', 'BLL', 'BAC', 'BBWI', 'BAX', 'BDX','WRB',
    'BRK.B', 'BBY', 'BIO', 'TECH', 'BIIB', 'BLK', 'BK', 'BA', 'BKNG', 'BWA', 'BXP', 'BSX', 'BMY', 'AVGO', 'BR','BRO',
    'BF.B', 'CHRW', 'CDNS', 'CZR', 'CPT', 'CPB', 'COF', 'CAH', 'KMX', 'CCL', 'CARR', 'CTLT', 'CAT', 'CBOE','CBRE',
    'CDW', 'CE', 'CNC', 'CNP', 'CDAY', 'CERN', 'CF', 'CRL', 'SCHW', 'CHTR', 'CVX', 'CMG', 'CB', 'CHD', 'CI','CINF',
    'CTAS', 'CSCO', 'C', 'CFG', 'CTXS', 'CLX', 'CME', 'CMS', 'KO', 'CTSH', 'CL', 'CMCSA', 'CMA', 'CAG','COP', 'ED',
    'STZ', 'CEG', 'COO', 'CPRT', 'GLW', 'CTVA', 'COST', 'CTRA', 'CCI', 'CSX', 'CMI', 'CVS', 'DHI','DHR', 'DRI', 'DVA',
    'DE', 'DAL', 'XRAY', 'DVN', 'DXCM', 'FANG', 'DLR', 'DFS', 'DISH', 'DIS', 'DG', 'DLTR','D', 'DPZ', 'DOV', 'DOW', 'DTE',
    'DUK', 'DRE', 'DD', 'DXC', 'EMN', 'ETN', 'EBAY', 'ECL', 'EIX', 'EW', 'EA','EMR', 'ENPH', 'ETR', 'EOG', 'EPAM', 'EFX',
    'EQIX', 'EQR', 'ESS', 'EL', 'ETSY', 'RE', 'EVRG', 'ES', 'EXC','EXPE', 'EXPD', 'EXR', 'XOM', 'FFIV', 'FDS', 'FAST', 'FRT',
    'FDX', 'FITB', 'FRC', 'FE', 'FIS', 'FISV', 'FLT','FMC', 'F', 'FTNT', 'FTV', 'FBHS', 'FOXA', 'FOX', 'BEN', 'FCX', 'AJG',
    'GRMN', 'IT', 'GE', 'GNRC', 'GD', 'GIS','GPC', 'GILD', 'GL', 'GPN', 'GM', 'GS', 'GWW', 'HAL', 'HIG', 'HAS', 'HCA', 'PEAK',
    'HSIC', 'HSY', 'HES', 'HPE','HLT', 'HOLX', 'HD', 'HON', 'HRL', 'HST', 'HWM', 'HPQ', 'HUM', 'HII', 'HBAN', 'IEX', 'IDXX',
    'ITW', 'ILMN', 'INCY','IR', 'INTC', 'ICE', 'IBM', 'IP', 'IPG', 'IFF', 'INTU', 'ISRG', 'IVZ', 'IPGP', 'IQV', 'IRM', 'JBHT',
    'JKHY', 'J','JNJ', 'JCI', 'JPM', 'JNPR', 'K', 'KEY', 'KEYS', 'KMB', 'KIM', 'KMI', 'KLAC', 'KHC', 'KR', 'LHX', 'LH', 'LRCX',
    'LW', 'LVS', 'LDOS', 'LEN', 'LLY', 'LNC', 'LIN', 'LYV', 'LKQ', 'LMT', 'L', 'LOW', 'LUMN', 'LYB', 'MTB', 'MRO', 'MPC','MKTX',
    'MAR', 'MMC', 'MLM', 'MAS', 'MA', 'MTCH', 'MKC', 'MCD', 'MCK', 'MDT', 'MRK', 'FB', 'MET', 'MTD', 'MGM', 'MCHP','MU', 'MSFT',
    'MAA', 'MRNA', 'MHK', 'MOH', 'TAP', 'MDLZ', 'MPWR', 'MNST', 'MCO', 'MS', 'MOS', 'MSI', 'MSCI', 'NDAQ','NTAP', 'NFLX', 'NWL',
    'NEM', 'NWSA', 'NWS', 'NEE', 'NLSN', 'NKE', 'NI', 'NDSN', 'NSC', 'NTRS', 'NOC', 'NLOK', 'NCLH','NRG', 'NUE', 'NVDA', 'NVR',
    'NXPI', 'ORLY', 'OXY', 'ODFL', 'OMC', 'OKE', 'ORCL', 'OGN', 'OTIS', 'PCAR', 'PKG', 'PARA','PH', 'PAYX', 'PAYC', 'PYPL', 'PENN',
    'PNR', 'PEP', 'PKI', 'PFE', 'PM', 'PSX', 'PNW', 'PXD', 'PNC', 'POOL', 'PPG', 'PPL','PFG', 'PG', 'PGR', 'PLD', 'PRU', 'PEG', 'PTC',
    'PSA', 'PHM', 'PVH', 'QRVO', 'PWR', 'QCOM', 'DGX', 'RL', 'RJF', 'RTX', 'O','REG', 'REGN', 'RF', 'RSG', 'RMD', 'RHI', 'ROK', 'ROL',
    'ROP', 'ROST', 'RCL', 'SPGI', 'CRM', 'SBAC', 'SLB', 'STX', 'SEE','SRE', 'NOW', 'SHW', 'SBNY', 'SPG', 'SWKS', 'SJM', 'SNA', 'SEDG',
    'SO', 'LUV', 'SWK', 'SBUX', 'STT', 'STE', 'SYK', 'SIVB','SYF', 'SNPS', 'SYY', 'TMUS', 'TROW', 'TTWO', 'TPR', 'TGT', 'TEL', 'TDY',
    'TFX', 'TER', 'TSLA', 'TXN', 'TXT', 'TMO', 'TJX','TSCO', 'TT', 'TDG', 'TRV', 'TRMB', 'TFC', 'TWTR', 'TYL', 'TSN', 'USB', 'UDR',
    'ULTA', 'UAA', 'UA', 'UNP', 'UAL', 'UNH', 'UPS', 'URI', 'UHS', 'VLO', 'VTR', 'VRSN', 'VRSK', 'VZ', 'VRTX', 'VFC', 'VTRS', 'V', 'VNO',
    'VMC', 'WAB', 'WMT', 'WBA', 'WBD', 'WM', 'WAT', 'WEC', 'WFC', 'WELL', 'WST', 'WDC', 'WRK', 'WY', 'WHR', 'WMB', 'WTW', 'WYNN', 'XEL',
    'XYL', 'YUM', 'ZBRA', 'ZBH', 'ZION', 'ZTS'};

    std::thread marketDataThread(&IBClient::requestMarketData, &client, std::ref(sp500Stocks));
    std::thread historicalDataThread(&IBClient::requestHistoricalData, &client, std::ref(sp500Stocks));
    marketDataThread.join();
    historicalDataThread.join();
    std::this_thread::sleep_for(std::chrono::minutes(1));
    client.findTopDecreasingStocks();
    client.disconnect();
    return 0;
}
