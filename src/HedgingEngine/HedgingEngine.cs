using GrpcPricing.Protos;
using MarketData;
using ParameterInfo.JsonUtils;
using ParameterInfo;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ParameterInfo.RebalancingOracleDescriptions;
using TimeHandler;
using HedgingEngine.Helpers;

namespace HedgingEngine
{
    public class HedgingEngine
    {
        public Portfolio Portfolio_ { get; set; }

        public TestParameters TestParams_ { get; set; }

        public List<DataFeed> MarketData_ { get; set; }

        public GrpcPricer.GrpcPricerClient Client_ { get; set; }

        private readonly int NbDaysPerYear_;

        private readonly double InterestRate_;

        private readonly IRebalancingOracleDescription RebalancingDescr_;

        private readonly DateTime CreationDate_;

        private readonly DateTime[] PaymentDates_;


        private readonly MathDateConverter converter_;


        public HedgingEngine(string JsonDescr, string csvFilePath, GrpcPricer.GrpcPricerClient client)
        {
            StreamReader sr = new StreamReader(JsonDescr);
            TestParams_ = JsonIO.FromJson(sr.ReadToEnd());
            MarketData_ = MarketDataReader.ReadDataFeeds(csvFilePath);
            Portfolio_ = new Portfolio();
            Client_ = client;
            NbDaysPerYear_ = TestParams_.NumberOfDaysInOneYear;
            InterestRate_ = TestParams_.AssetDescription.CurrencyRates.Values.ToArray<double>()[0];
            RebalancingDescr_ = TestParams_.RebalancingOracleDescription;
            CreationDate_ = TestParams_.PayoffDescription.CreationDate;
            PaymentDates_ = TestParams_.PayoffDescription.PaymentDates;
            converter_ = new MathDateConverter(NbDaysPerYear_);
        }



        public void HedgePortfolio(string outPutPath)
        {
            HelperMethods _helper = new HelperMethods();
            PastLines pastline = new PastLines();
            pastline.Value.Add(MarketData_[0].SpotList.Values.ToArray<double>());
            PricingInput input = _helper.createInput(0, false, new List<PastLines> { pastline });

            PricingOutput output = Client_.PriceAndDeltas(input);
            OutputData firstOutPut = new OutputData();

            Dictionary<string, int> shareIds = new Dictionary<string, int>();
            Dictionary<string, double> initialComposition = new Dictionary<string, double>();
            _helper.GetIdsAndShareSpots(shareIds, MarketData_[0].SpotList);
            _helper.SetComposition(initialComposition, shareIds, output.Deltas.ToArray());

            Portfolio_ = new Portfolio(initialComposition, output.Price, CreationDate_, 0, CreationDate_);
            Portfolio_.RiskFreeCash_ = Portfolio_.PortfolioValue_ - _helper.ComputeSumProd(Portfolio_.Composition_,MarketData_[0].SpotList);
            
            string description = "";
            _helper.fillOutput(firstOutPut, output, Portfolio_, MarketData_[0]);
            description += "[";
            description += ParameterInfo.JsonUtils.JsonIO.ToJson(firstOutPut);



            for (int index = 1; index < MarketData_.Count; index++)
            {

                if (!input.MonitoringDateReached)
                {
                    _helper.RemoveLastPastLineFromInput(input);
                }
                _helper.AddPastLineToInput(input, MarketData_, index);
                input.MonitoringDateReached = PaymentDates_.Contains(MarketData_[index].Date);

                input.Time = converter_.ConvertToMathDistance(CreationDate_, MarketData_[index].Date);
                if (_helper.IsRebalancingTime(index, RebalancingDescr_))
                {
                    output = Client_.PriceAndDeltas(input);
                    Portfolio_.RebalancePortfolio(MarketData_[index], output, InterestRate_, NbDaysPerYear_, _helper);
                    firstOutPut = new OutputData();
                    _helper.fillOutput(firstOutPut, output, Portfolio_, MarketData_[index]);
                    description += ",\n";
                    description += ParameterInfo.JsonUtils.JsonIO.ToJson(firstOutPut);
                }
            }
            description += "]";
            File.WriteAllText(outPutPath, description);
        }
    }
}
