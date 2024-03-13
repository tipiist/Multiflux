using Grpc.Net.Client;

using MarketData;
using ParameterInfo;
using ParameterInfo.JsonUtils;
using ParameterInfo.MarketDescription;
using System.Data;
using TimeHandler;
using ParameterInfo.RebalancingOracleDescriptions;
using Google.Protobuf;
using Google.Protobuf.Collections;
using MarketDataGeneration;
using GrpcPricing.Protos;

namespace HedgingEngine
{
    public class Program
    {
        static void Main(string[] args)
        {
            //déclaration client 

            var httpHandler = new HttpClientHandler();
            // Return `true` to allow certificates that are untrusted/invalid
            httpHandler.ServerCertificateCustomValidationCallback =
                HttpClientHandler.DangerousAcceptAnyServerCertificateValidator;
            using var channel = GrpcChannel.ForAddress("http://localhost:50051",
                new GrpcChannelOptions { HttpHandler = httpHandler });
            var client = new GrpcPricer.GrpcPricerClient(channel);

            string JsonDescr = args[0];
            StreamReader sr = new StreamReader(JsonDescr);
            string str = sr.ReadToEnd();
            TestParameters testParams = JsonIO.FromJson(str);

            string csvFilePath = args[1];
            string outPutPath = args[2];
            HedgingEngine hedger = new HedgingEngine(JsonDescr, csvFilePath, client);
            hedger.HedgePortfolio(outPutPath);
        }
    }
}
