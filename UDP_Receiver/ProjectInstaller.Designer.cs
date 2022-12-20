
namespace LoRaReceiver
{
    partial class ProjectInstaller
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary> 
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Component Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.LoRaRevceiverInstaller = new System.ServiceProcess.ServiceProcessInstaller();
            this.LoRaServiceInstaller = new System.ServiceProcess.ServiceInstaller();
            // 
            // LoRaRevceiverInstaller
            // 
            this.LoRaRevceiverInstaller.Password = null;
            this.LoRaRevceiverInstaller.Username = null;
            // 
            // LoRaServiceInstaller
            // 
            this.LoRaServiceInstaller.ServiceName = "LoRaReceiverService";
            // 
            // ProjectInstaller
            // 
            this.Installers.AddRange(new System.Configuration.Install.Installer[] {
            this.LoRaRevceiverInstaller,
            this.LoRaServiceInstaller});

        }

        #endregion

        private System.ServiceProcess.ServiceProcessInstaller LoRaRevceiverInstaller;
        private System.ServiceProcess.ServiceInstaller LoRaServiceInstaller;
    }
}