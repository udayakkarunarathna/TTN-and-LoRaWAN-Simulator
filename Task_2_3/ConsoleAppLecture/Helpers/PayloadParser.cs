using System;
using System.Globalization;

namespace ConsoleAppLecture.Helpers
{
    public static class PayloadParser
    {
        public static (double Temp, double Hum, double Led) Parse(string ascii)
        {
            if (string.IsNullOrWhiteSpace(ascii))
                throw new FormatException("Empty payload.");

            // Normalize: remove spaces so "Temp:18.4, Hum: 38.7, LED: 0.72" -> "Temp:18.4,Hum:38.7,LED:0.72"
            var normalized = ascii.Replace(" ", string.Empty);

            var parts = normalized.Split(',');
            if (parts.Length < 3)
                throw new FormatException("Payload does not contain three comma-separated parts.");

            var ci = CultureInfo.InvariantCulture;

            try
            {
                double temp = double.Parse(parts[0].Split(':')[1], ci);
                double hum = double.Parse(parts[1].Split(':')[1], ci);
                double led = double.Parse(parts[2].Split(':')[1], ci);
                return (temp, hum, led);
            }
            catch (Exception ex)
            {
                throw new FormatException("Payload format invalid: " + ex.Message, ex);
            }
        }
    }
}
