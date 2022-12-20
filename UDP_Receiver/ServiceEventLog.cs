using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Diagnostics;

namespace LoRaReceiver
{
    class ServiceEventLog
    {
		public static void WriteInfo(string strWarning)
		{
			string strSource = "LoRaSensorListener";
			string strLog = "Application";

			if (!EventLog.SourceExists(strSource))
			{
				EventLog.CreateEventSource(strSource, strLog);
			}

			EventLog.WriteEntry(strSource, strWarning, EventLogEntryType.Information);
		}
	}
}
