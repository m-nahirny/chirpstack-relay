using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Linq;
using System.ServiceProcess;
using System.Text;
using System.Threading.Tasks;
using System.Net;
using System.Net.Sockets;
using System.Configuration;
using System.IO;
using System.Reflection;
using Newtonsoft.Json;

namespace LoRaReceiver
{
    public partial class LoRaReceiverService : ServiceBase
    {
        private int listenPort = 40000;
        private int debugLevel = 0;
        private UdpClient listener;
        private Task task;
        private UdpState state;

        public struct UdpState
        {
            public UdpClient u;
            public IPEndPoint e;
        }

        public LoRaReceiverService()
        {
            InitializeComponent();
        }

        protected override void OnStart(string[] args)
        {
            task = Task.Run(() => WaitforMessage());
            ServiceEventLog.WriteInfo(string.Format("Service started - listening on port {0} debug level {1}", listenPort, debugLevel));
        }

        protected override void OnStop()
        {
        }

        protected void WaitforMessage()
        {

            //           listener = new UdpClient(listenPort);
            //groupEP = new IPEndPoint(IPAddress.Any, listenPort);

            IPEndPoint e = new IPEndPoint(IPAddress.Any, listenPort);
            listener = new UdpClient(e);

            state = new UdpState();
            state.e = e;
            state.u = listener;

            try
            {
                listener.BeginReceive(new AsyncCallback(ReceiveMessage), state);
            }
            catch (Exception ex)
            {
                ServiceEventLog.WriteInfo(string.Format("WaitforMessage exception - {0}", ex.Message));
            }
        }

        private void ReceiveMessage(IAsyncResult res)
        {
            string strReceived;

            try
            {
                ServiceEventLog.WriteInfo("Received a message");
                UdpClient u = ((UdpState)(res.AsyncState)).u;
                IPEndPoint e = ((UdpState)(res.AsyncState)).e;

                byte[] receiveBytes = u.EndReceive(res, ref e);
                strReceived = Encoding.ASCII.GetString(receiveBytes);

                // wait for next message
                listener.BeginReceive(new AsyncCallback(ReceiveMessage), state);

                ServiceEventLog.WriteInfo(string.Format("Received broadcast from {0} : {1}", e.ToString(), strReceived));

                UDP_Message msg = JsonConvert.DeserializeObject<UDP_Message>(strReceived);
                using (LoRaMessagesEntities context = new LoRaMessagesEntities())
                {
                    Message newMessage = new Message();
                    newMessage.MessageContents = msg.message;
                    //DateTime utcTime = DateTime.Parse(msg.time);
                    TimeZoneInfo meTimeZone = TimeZoneInfo.FindSystemTimeZoneById("Pacific Standard Time");
                    newMessage.ReceivedTime = TimeZoneInfo.ConvertTimeFromUtc(msg.time, meTimeZone);
                    newMessage.RSSI = msg.rssi;
                    newMessage.SNR = msg.snr;
                    newMessage.SpreadingFactor = msg.sf;
                    newMessage.BatteryVoltage = msg.battery;
                    context.Messages.Add(newMessage);
                    context.SaveChanges();
                }
            }
            catch (Exception ex)
            {
                ServiceEventLog.WriteInfo("ReceiveMessage Exception: " + ex.Message);
            }
        }
    }
}
