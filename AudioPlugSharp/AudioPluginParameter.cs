﻿using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Text;

namespace AudioPlugSharp
{
    public enum EAudioPluginParameterType
    {
        Float,
        Bool,
        Int,
        Enum
    }

    public class AudioPluginParameter : INotifyPropertyChanged
    {
        public IAudioPluginEditor Editor { get; internal set; } 

        public event PropertyChangedEventHandler PropertyChanged;

        public int ParameterIndex { get; internal set; }
        public string ID { get; set; }
        public string Name { get; set; }
        public EAudioPluginParameterType Type { get; set; }
        public double MinValue { get; set; }
        public double MaxValue { get; set; }
        public double DefaultValue { get; set; }
        public string ValueFormat { get; set; }

        public delegate void ValueChangedHandler(AudioPluginParameter sender);
        public event ValueChangedHandler ValueChanged;

        public double Value
        {
            get { return value; }
            set
            {
                if (this.value != value)
                {
                    this.value = value;

                    ValueChanged?.Invoke(this);

                    OnPropertyChanged("Value");
                    OnPropertyChanged("EditValue");
                    OnPropertyChanged("DisplayValue");
                    OnPropertyChanged("NormalizedValue");
                }
            }
        }

        // To be used by UI controls to change the value
        public double EditValue
        {
            get
            {
                return value;
            }
            set
            {
                this.Value = value;

                // This should really be exposed to UI controls so multiple edits can be inside a begin/end edit
                Editor.Host.BeginEdit(ParameterIndex);
                Editor.Host.PerformEdit(ParameterIndex, GetValueNormalized(value));
                Editor.Host.EndEdit(ParameterIndex);
            }
        }

        public string DisplayValue { get { return String.Format(ValueFormat, Value); } }

        public double NormalizedValue
        {
            get { return GetValueNormalized(Value); }
            set { Value = GetValueDenormalized(value); }
        }

        double value;

        public AudioPluginParameter()
        {
            MinValue = 0;
            MaxValue = 1;
            DefaultValue = 0.5;
            ValueFormat = "{0:0.0}";
        }

        public double GetValueNormalized(double value)
        {
            return (value - MinValue) / (MaxValue - MinValue);
        }

        public double GetValueDenormalized(double value)
        {
            return MinValue + ((MaxValue - MinValue) * value);
        }

        public void OnPropertyChanged(string name)
        {
            PropertyChangedEventHandler handler = PropertyChanged;
            if (handler != null)
            {
                handler(this, new PropertyChangedEventArgs(name));
            }
        }
    }
}
