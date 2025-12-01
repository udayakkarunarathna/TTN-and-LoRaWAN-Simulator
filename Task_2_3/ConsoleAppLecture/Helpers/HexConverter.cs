using System;
using System.Text;
using System.Text.RegularExpressions;

namespace ConsoleAppLecture.Helpers
{
    public static class HexConverter
    {
        public static string HexToAscii(string hex)
        {
            if (string.IsNullOrWhiteSpace(hex))
                throw new ArgumentException("Hex string cannot be null or empty.", nameof(hex));

            // Remove allowed separators only
            string cleaned = hex.Replace("-", "").Replace(" ", "");

            // Validate: must be even length
            if (cleaned.Length % 2 != 0)
                throw new ArgumentException("Hex string length must be even.", nameof(hex));

            // Validate: must contain only hex characters
            if (!Regex.IsMatch(cleaned, @"\A[0-9A-Fa-f]*\z"))
                throw new FormatException("Invalid hex character detected.");

            var sb = new StringBuilder(cleaned.Length / 2);

            for (int i = 0; i < cleaned.Length; i += 2)
            {
                string pair = cleaned.Substring(i, 2);
                byte b = Convert.ToByte(pair, 16);
                sb.Append((char)b);
            }

            return sb.ToString();
        }
    }
}
