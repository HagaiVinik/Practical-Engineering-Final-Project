using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Linq.Expressions;
using System.Net;
using System.Net.Sockets;
using System.Runtime.Remoting.Messaging;
using System.Text;
using System.Threading;
using System.Windows.Controls.Primitives;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Threading;

namespace PstnDiagGUI01
{
    class PSTNDiagViewModel : ViewModelBase
    {       
        public bool D_channel_1_flag;
        public bool D_channel_2_flag;

        private string _logText;
        public string logText
        {
            get { return _logText; }
            set
            {
                _logText = value;
                RaisePropertyChanged("logText");
            }
        }

        private SolidColorBrush _Connection;
        public SolidColorBrush Connection
        {
            get { return _Connection; }
            set
            {
                _Connection = value;
                RaisePropertyChanged("Connection");
            }
        }
        private SolidColorBrush _Connection2;
        public SolidColorBrush Connection2
        {
            get { return _Connection2; }
            set
            {
                _Connection2 = value;
                RaisePropertyChanged("Connection2");
            }
        }

        private SolidColorBrush _DChannelTrunk_1;
        public SolidColorBrush DChannelTrunk_1
        {
            get { return _DChannelTrunk_1; }
            set
            {
                _DChannelTrunk_1 = value;
                RaisePropertyChanged("DChannelTrunk_1");
            }
        }
        private SolidColorBrush _DChannelTrunk_2;
        public SolidColorBrush DChannelTrunk_2
        {
            get { return _DChannelTrunk_2; }
            set
            {
                _DChannelTrunk_2 = value;
                RaisePropertyChanged("DChannelTrunk_2");
            }
        }

        public ObservableCollection<PstnDiagObjectModel> Trunk1_1 { get; set; }     /* First line in GUI of trunk 1 */
        public ObservableCollection<PstnDiagObjectModel> Trunk1_2 { get; set; }     /* Second */
        public ObservableCollection<PstnDiagObjectModel> Trunk2_1 { get; set; }     /* First line in GUI of trunk 2 */ 
        public ObservableCollection<PstnDiagObjectModel> Trunk2_2 { get; set; }     /* Second */

        public Dictionary<int, PstnDiagObjectModel> Collection;     /* Dictionary for choosing a slot/button to update */
        public int id = 6;                              /* id for dictionary */
        public int slotNumber = 1;                      /* counter of timeslots */

        private PstnDiagObjectModel _picked;           /* ?? used for binding on click ? */
        public PstnDiagObjectModel Picked
        {
            get { return _picked; }
            set
            {
                _picked = value;
                RaisePropertyChanged("Picked");
            }
        }
        
        private ICommand _command;                 /* used for binding to click on line button */
        public ICommand DisplayCommand
        {
            get { return _command; }
            set { _command = value; }
        }

        private ICommand _UnblockCommand;                 
        public ICommand UnblockCommand
        {
            get { return _UnblockCommand; }
            set { _UnblockCommand = value; }
        }

        private ICommand _BlockCommand;                 
        public ICommand BlockCommand
        {
            get { return _BlockCommand; }
            set { _BlockCommand = value; }
        }

        private ICommand _ResumeCommand;                 
        public ICommand ResumeCommand
        {
            get { return _ResumeCommand; }
            set { _ResumeCommand = value; }
        }

        private ICommand _HoldCommand;                
        public ICommand HoldCommand
        {
            get { return _HoldCommand; }
            set { _HoldCommand = value; }
        }

        private ICommand _DisconnectCommand;                 
        public ICommand DisconnectCommand
        {
            get { return _DisconnectCommand; }
            set { _DisconnectCommand = value; }
        }

        private ICommand _ResetCommand;
        public ICommand ResetCommand
        {
            get { return _ResetCommand; }
            set { _ResetCommand = value; }
        }

        private ICommand _OpenAllCommand;
        public ICommand OpenAllCommand
        {
            get { return _OpenAllCommand; }
            set { _OpenAllCommand = value; }
        }

        private ICommand _CloseAllCommand;
        public ICommand CloseAllCommand
        {
            get { return _CloseAllCommand; }
            set { _CloseAllCommand = value; }
        }

        /* connection to service */
        public Thread T1;
        /*private TcpClient Tcpclient;*/

        public Socket Socket;
        public Socket Socket_send;
        public int Port = 5000;
        public int PortSend= 6000;
        public string Ip = "127.0.0.1";
        public byte[] message_to_server;

        public PSTNDiagViewModel()   /* Constructor */
        {

            Trunk1_1 = new ObservableCollection<PstnDiagObjectModel>();
            Trunk1_2 = new ObservableCollection<PstnDiagObjectModel>();
            Trunk2_1 = new ObservableCollection<PstnDiagObjectModel>();
            Trunk2_2 = new ObservableCollection<PstnDiagObjectModel>();

            Collection = new Dictionary<int, PstnDiagObjectModel>();

            _command = new RelayCommand(Show, canExecute);
            _UnblockCommand = new RelayCommand(send_unblock_command, canExecute);
            _BlockCommand = new RelayCommand(send_block_command, canExecute);
            _ResumeCommand = new RelayCommand(send_resume_command, canExecute);
            _HoldCommand = new RelayCommand(send_hold_command, canExecute);
            _DisconnectCommand = new RelayCommand(send_disconnect_command, canExecute);
            _ResetCommand = new RelayCommand(send_reset_command, canExecute);
            _OpenAllCommand = new RelayCommand(send_openAll_command, canExecute);
            _CloseAllCommand = new RelayCommand(send_closeAll_command, canExecute);

            InitObject(Trunk1_1,1);
            InitObject(Trunk1_2,1);
            InitObject(Trunk2_1,2);
            InitObject(Trunk2_2,2);

            logText = "LOG:\n";

            message_to_server = null;

            message_to_server = new byte[128];

            D_channel_1_flag = false;
            D_channel_2_flag = false;

            bool s1 = start_socket();      /* connect to socket */
            
            SolidColorBrush a = new SolidColorBrush();
            a.Color = Brushes.Red.Color;
            Connection = a;
            Connection2 = a;
            SetDchannelTrunk_1(a);
            SetDchannelTrunk_2(a);
        }

        public void updateTextBox(string logData)
        {
            Dispatcher.CurrentDispatcher.Invoke(new Action(() => logText = logText + logData + "\n"));
        }

        public void InitObject(ObservableCollection<PstnDiagObjectModel> tempTrunk ,int trunkNum)
        {
            int MaxNumSlots = 15;  /* num of slots */
            for (int i = 1; i <= MaxNumSlots; i++)
            {
                SolidColorBrush a = new SolidColorBrush();
                a.Color = Brushes.IndianRed.Color;
                PstnDiagObjectModel p1 = new PstnDiagObjectModel
                {
                    ChannelState = "Blocked",
                    TrunkNum = trunkNum,
                    TimeSlotNumber = slotNumber,
                    Name = "slot" + slotNumber,
                    Color = a,
                    CallState = "Idle",
                    ID = id
                };
                
                Collection.Add(id,p1);
                id = id + 3;
                tempTrunk.Add(p1);
                slotNumber++;
            }
            if (slotNumber > 30)
            {
                slotNumber = 1;
            }
            Picked = Collection[6];
        }

        public void SetDchannelTrunk_1(SolidColorBrush color)
        {
            color.Freeze();
           Dispatcher.CurrentDispatcher.Invoke(new Action(() => DChannelTrunk_1 = color));
        }

        public void SetDchannelTrunk_2(SolidColorBrush color)
        {
            color.Freeze();
            Dispatcher.CurrentDispatcher.Invoke(new Action(() => DChannelTrunk_2 = color));
        }

        /* functions triggered when one of option buttons clicked */

        public void Show(object param)
        {
            Picked = Collection[Convert.ToInt16(param)];
        }

        public bool canExecute(object param)
        {
            return true;
        }

        public void send_unblock_command(object param)
        {
            Console.WriteLine("Unblock button clicked");
            string msg = "";
            if (Collection[Picked.ID].ChannelState != "Unblocked")
            {
                msg = "Unblock" + ":" + Convert.ToString(Picked.TrunkNum) + ":" + Convert.ToString(Picked.TimeSlotNumber);
                message_to_server = Encoding.ASCII.GetBytes(msg);              
                sendToServer(msg);     
            }
        }

        public void send_block_command(object param)
        {
            Console.WriteLine("Block button clicked");
            string msg = "";
            if (Collection[Picked.ID].ChannelState != "Blocked")
            {
                SolidColorBrush d = new SolidColorBrush();
                d.Color = Brushes.IndianRed.Color;
                paintButton(Picked.ID, d, "Blocked", "Idle");
                msg = "Block" + ":" + Convert.ToString(Picked.TrunkNum) + ":" + Convert.ToString(Picked.TimeSlotNumber);
                message_to_server = Encoding.ASCII.GetBytes(msg);
                sendToServer(msg);
            }
        }

        public void send_resume_command(object param)
        {
            Console.WriteLine("Resume button clicked");
            string msg = "";
            if (Collection[Picked.ID].CallState == "SoundBlocked")
            {
                SolidColorBrush d = new SolidColorBrush();
                d.Color = Brushes.Purple.Color;
                paintButton(Picked.ID, d, "Unblocked", "Connected");
                if (Picked.ID > 93)
                    paintButton(Picked.ID - 90, d, "Unblocked", "Connected");
                else
                    paintButton(Picked.ID + 90, d, "Unblocked", "Connected");
                msg = "Resume" + ":" + Convert.ToString(Picked.TrunkNum) + ":" + Convert.ToString(Picked.TimeSlotNumber);
                message_to_server = Encoding.ASCII.GetBytes(msg);
                sendToServer(msg);
            }
        }

        public void send_hold_command(object param)
        {
            Console.WriteLine("Hold button clicked");
            string msg = "";
            if (Collection[Picked.ID].CallState == "Connected")
            {
                SolidColorBrush d = new SolidColorBrush();
                d.Color = Brushes.DarkOrange.Color;
                paintButton(Picked.ID, d, "Unblocked", "SoundBlocked");
                if (Picked.ID > 93)
                    paintButton(Picked.ID-90, d, "Unblocked", "SoundBlocked");
                else
                    paintButton(Picked.ID + 90, d, "Unblocked", "SoundBlocked");

                msg = "Hold" + ":" + Convert.ToString(Picked.TrunkNum) + ":" + Convert.ToString(Picked.TimeSlotNumber);
                message_to_server = Encoding.ASCII.GetBytes(msg);
                sendToServer(msg);
            }
        }

        public void send_disconnect_command(object param)
        {
            Console.WriteLine("Disconnect button clicked");
            string msg = "";
            if (Collection[Picked.ID].CallState == "Connected")
            {
                SolidColorBrush d = new SolidColorBrush();
                d.Color = Brushes.LightGreen.Color;
                paintButton(Picked.ID, d, "Unblocked", "Idle");
                msg = "Disconnect" + ":" + Convert.ToString(Picked.TrunkNum) + ":" + Convert.ToString(Picked.TimeSlotNumber);
                message_to_server = Encoding.ASCII.GetBytes(msg);
                sendToServer(msg);
            }
        }

        public void send_reset_command(object param)
        {
            Console.WriteLine("Reset button clicked");
            string msg = "";
            if (Collection[Picked.ID].ChannelState == "Unblocked")
            {
                SolidColorBrush d = new SolidColorBrush();
                d.Color = Brushes.IndianRed.Color;
                int temp_id = 6;
                while (temp_id < 184)
                {
                    paintButton(temp_id, d, "Blocked", "Idle");
                    temp_id = temp_id + 3;
                }

                msg = "Reset" + ":" + Convert.ToString(Picked.TrunkNum) + ":" + Convert.ToString(Picked.TimeSlotNumber);
                message_to_server = Encoding.ASCII.GetBytes(msg);
                sendToServer(msg);
            }
        }

        public void send_openAll_command(object param)
        {
            Console.WriteLine("OpenAll button clicked");
            string msg = "";
            if (Collection[Picked.ID].ChannelState == "Blocked")
            {
                msg = "OpenAll" + ":" + Convert.ToString(Picked.TrunkNum) + ":" + Convert.ToString(Picked.TimeSlotNumber);
                message_to_server = Encoding.ASCII.GetBytes(msg);
                sendToServer(msg);
            }
        }

        public void send_closeAll_command(object param)
        {
            Console.WriteLine("CloseAll button clicked");
            string msg = "";
            if (Collection[Picked.ID].ChannelState == "Unblocked")
            {
                SolidColorBrush d = new SolidColorBrush();
                d.Color = Brushes.IndianRed.Color;
                int temp_id = 6;
                while (temp_id < 184)
                {
                    paintButton(temp_id, d, "Blocked", "Idle");
                    temp_id = temp_id + 3;
                }

                msg = "CloseAll" + ":" + Convert.ToString(Picked.TrunkNum) + ":" + Convert.ToString(Picked.TimeSlotNumber);
                message_to_server = Encoding.ASCII.GetBytes(msg);
                sendToServer(msg);
            }
        }

        /* socket handeling functions */

        public bool start_socket()
        {
            bool success = false;
            Socket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            IPEndPoint endPoint = new IPEndPoint(IPAddress.Parse(Ip),Port);
            try
            {
                Socket.Connect(endPoint);
                Console.WriteLine("connected");
                Thread t = new Thread(() => receiveDataFromSocket());
                t.Start();
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.ToString());
            }
            return success;
        }

        public void receiveDataFromSocket()
        {
            byte[] buffer = new byte[128];
            int recv = 0;
            string[] array;
            string eventType ;
            int id ;
            int trunk ;
            int timeSlot ;
            while (true)
            {
                buffer = new byte[128];
                try
                {
                    recv = Socket.Receive(buffer, 0, buffer.Length, 0);
                }
                catch (Exception)
                {
                    Console.WriteLine("Socket forcely Closed ");
                };
                if (recv < 1)
                {
                    Console.WriteLine("Note: Server disconnected. ");
                    return;
                }
                string result = Encoding.ASCII.GetString(buffer, 0, recv);
                
                Console.WriteLine(result);
                try
                {
                    array = result.Split(':');
                    eventType = array[0];
                    id = Convert.ToInt16(array[1]);
                    trunk = Convert.ToInt16(array[2]);
                    timeSlot = Convert.ToInt32(array[3]);
                }
                catch (Exception)
                {
                    Console.WriteLine("corupted message recieved from server ");
                    return;
//                    continue;
                }
                               
                
                SolidColorBrush d = new SolidColorBrush();

                switch (eventType)
                {
                    case "GCEV_D_CHAN_STATUS":                         
                        //Console.WriteLine("***NOTE -  D Channel status Changed***");
                        
                        if (trunk == 1)
                        {
                            if (timeSlot == 2) // D Channel on.
                            {
                                d.Color = Brushes.Green.Color;
                                SetDchannelTrunk_1(d);
                            }

                            else if (timeSlot == 1)
                            {
                                d.Color = Brushes.Red.Color;
                                SetDchannelTrunk_1(d);    
                            }
                        }
                        
                        if (trunk == 2)
                        {
                            if (timeSlot == 2)
                            {
                                d.Color = Brushes.Green.Color;
                                SetDchannelTrunk_2(d);
                            }
                            else if (timeSlot == 1)
                            {
                                d.Color = Brushes.Red.Color;
                                SetDchannelTrunk_2(d);    
                            }                           
                        }
                        break;

                    case  "GCEV_UNBLOCKED":
                        d.Color = Brushes.Green.Color;
                        if (id == 2 )
                        {
                            d.Freeze();
                            Dispatcher.CurrentDispatcher.Invoke(new Action(() => Connection = d));
                            break;
                        }
                        if (id == 4 )
                        {
                            d.Freeze();
                            Dispatcher.CurrentDispatcher.Invoke(new Action(() => Connection2 = d));                        
                            break;
                        }

                        d.Color = Brushes.LightGreen.Color;
                        d.Freeze();
                        paintButton(id, d, "Unblocked", "Idle");
                        break;

                    case "GCEV_BLOCKED":
                        d.Color = Brushes.Red.Color;
                        if (id == 2)
                        {
                            d.Freeze();
                            Dispatcher.CurrentDispatcher.Invoke(new Action(() => Connection = d)); 
                            break;
                        }
                        if (id == 4)
                        {
                            d.Freeze();
                            Dispatcher.CurrentDispatcher.Invoke(new Action(() => Connection2 = d));                          
                            break;
                        }
                        d.Color = Brushes.IndianRed.Color;
                        paintButton(id, d, "Blocked","Idle");
                        break;

                    case "GCEV_OFFERED":
                        d.Color = Brushes.Plum.Color;
                        paintButton(id, d, "Unblocked","Offered");
                        break;

                    case "GCEV_CONNECTED":
                        d.Color = Brushes.Purple.Color;
                        paintButton(id, d, "Unblocked", "Connected");
                        break;

                    case "GCEV_ANSWERED":
                        d.Color = Brushes.Purple.Color;
                        paintButton(id, d, "Unblocked", "Connected");
                        break;

                    case "GCEV_DISCONNECTED":
                        d.Color = Brushes.LightGreen.Color;
                        paintButton(id, d, "Unblocked", "Idle");
                        break;
                    default:
                        break;
                }
                updateTextBox(result);
            }
        }

        public void sendToServer( string msg)
        {
            Socket_send = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            IPEndPoint endPoint = new IPEndPoint(IPAddress.Parse(Ip), PortSend);
            try
            {
                Socket_send.Connect(endPoint);
                Console.WriteLine("Connected to: Send_Port");
                Console.WriteLine("Message to Server = " + msg);
                Socket_send.Send(message_to_server, 0, message_to_server.Length, 0);
                Console.WriteLine("Message sent");
            }
            catch (Exception ex)
            {
                Console.WriteLine("Error: " + ex.ToString());
            }
            Socket_send.Close();
            message_to_server = null;
        }

        public void paintButton(int id , SolidColorBrush d , string channelState , string callState)
        {
            d.Freeze();
            Dispatcher.CurrentDispatcher.Invoke(new Action(() => Collection[id].Color = d));
            Collection[id].ChannelState = channelState;
            Collection[id].CallState = callState;
            Picked = Collection[id];
        }
    }
}
