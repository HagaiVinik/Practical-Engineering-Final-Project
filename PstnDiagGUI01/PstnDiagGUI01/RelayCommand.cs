using System;
using System.Collections.Generic;
using System.Linq;
using System.Security.Cryptography.X509Certificates;
using System.Text;
using System.Windows.Input;

namespace PstnDiagGUI01
{
    class RelayCommand : ICommand
    {
        private Action<object> executeAction;
        private Func<object, bool> isOk;


        public RelayCommand(Action<object> executeAction, Func<object, bool> isOk)
        {
            this.isOk = isOk;
            this.executeAction = executeAction;
        }

        public event EventHandler CanExecuteChanged
        {
            add { CommandManager.RequerySuggested += value; }
            remove { CommandManager.RequerySuggested -= value; }
        }

        public bool CanExecute(object parameter)
        {
            if (isOk != null)
            {
                return true;
            }
            else
            {
                return isOk(parameter);
            }
        }

        public void Execute(object parameter)
        {
            executeAction(parameter);
        }
    }


}
