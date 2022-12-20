using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace LoRaReceiver
{
    class UDP_Message
    {
        public string message { get; set; }
        public int rssi { get; set; }
        public int sf { get; set; }
        public double snr { get; set; }
        public DateTime time { get; set; }
        public double battery { get; set; }
    }
}
