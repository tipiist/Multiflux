using Google.Protobuf.Collections;
using GrpcPricing.Protos;
using MarketData;
using ParameterInfo;
using ParameterInfo.RebalancingOracleDescriptions;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using TimeHandler;

namespace HedgingEngine.Helpers
{
    public class HelperMethods
    {
        public PricingInput createInput(double time, bool isMonitoringDate, List<PastLines>? past) 
        {
            PricingInput input = new PricingInput();
            input.Time = time;
            input.MonitoringDateReached = isMonitoringDate;
            input.Past.AddRange(past);
            return input;
        }
        public void AddPastLine(List<PastLines> pastLines, List<DataFeed> marketDataFeeds, int indexMarketData) {
            PastLines pastline = new PastLines();
            pastline.Value.Add(marketDataFeeds[indexMarketData].SpotList.Values.ToArray<double>());
            pastLines.Add(pastline);
        }
        public void AddPastLineToInput(PricingInput input, List<DataFeed> marketDataFeeds, int indexMarketData)
        {
            PastLines pastline = new PastLines();
            pastline.Value.Add(marketDataFeeds[indexMarketData].SpotList.Values.ToArray<double>());
            input.Past.Add(pastline);
        }

        public void RemoveLastPastLineFromInput(PricingInput input)
        {
            input.Past.RemoveAt(input.Past.Count - 1);
        }

        public bool IsRebalancingTime(DataFeed feed, FixedTimesOracleDescription fixedDescr, DateTime lastRebDate)
        {
            return DayCount.CountBusinessDays(lastRebDate, feed.Date) == fixedDescr.Period;
        }

        public void fillOutput(OutputData outputData, PricingOutput output, Portfolio portfolio, DataFeed feed)
        {
            outputData.Date = feed.Date;
            outputData.Deltas = output.Deltas.ToArray<double>();
            outputData.DeltasStdDev = output.DeltasStdDev.ToArray<double>();
            outputData.PriceStdDev = output.PriceStdDev;
            outputData.Price = output.Price;
            outputData.Value = portfolio.PortfolioValue_;
        }
        public void SetComposition(Dictionary<string, double> composition, Dictionary<string, int> shareIds, double[] deltas)
        {
            foreach (var element in shareIds)
            {
                composition[element.Key] = deltas[element.Value];
            }
        }
        public void GetIdsAndShareSpots(Dictionary<string, int> shareIds, Dictionary<string, double> spots)
        {
            int index = 0;
            foreach (var element in spots)
            {
                shareIds[element.Key] = index;
                index++;
            }
        }

        public double ComputeSumProd(Dictionary<string, double> composition, Dictionary<string, double> spots)
        {
            var sum = 0.0;
            foreach (var key in composition.Keys)
            {
                sum += composition[key] * spots[key];
            }
            return sum;
        }
        public bool IsRebalancingTime(int index, IRebalancingOracleDescription rebalancingOracle)
        {
            if(rebalancingOracle.Type == RebalancingOracleType.Fixed) { return (index % ((FixedTimesOracleDescription)rebalancingOracle).Period == 0); }
            return false;
        }
    }
}
