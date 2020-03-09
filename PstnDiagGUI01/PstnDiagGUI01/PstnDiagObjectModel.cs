using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Media;

namespace PstnDiagGUI01
{
    class PstnDiagObjectModel : ViewModelBase
    {
        /* Data Variables */
        public int ChannelID;
        public int TrunkNum { get; set; }
        /* GUI variables */
        public string Name { get; set; }
        public int TimeSlotNumber { get; set; }
        
        private SolidColorBrush _Color;
        public SolidColorBrush Color
        {
            get { return _Color; }
            set
            {
                _Color = value;
                RaisePropertyChanged("Color");
            }
        }
        public string ChannelState { get; set; }
        public string CallState { get; set; }
        public int ID { get; set; }
        
    }

}
