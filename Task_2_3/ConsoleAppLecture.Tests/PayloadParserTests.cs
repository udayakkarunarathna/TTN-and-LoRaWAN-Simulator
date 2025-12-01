using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using ConsoleAppLecture.Helpers;
using Xunit;

namespace ConsoleAppLecture.Tests
{
    public class PayloadParserTests
    {
        [Fact]
        public void Parse_ShouldReturnValues_WhenValidAscii()
        {
            // Arrange
            string ascii = "Temp:20.5,Hum:30.0,LED:0.20";

            // Act
            var result = PayloadParser.Parse(ascii);

            // Assert
            Assert.Equal(20.5, result.Temp);
            Assert.Equal(30.0, result.Hum);
            Assert.Equal(0.20, result.Led);
        }

        [Fact]
        public void Parse_ShouldHandleSpaces_WhenAsciiHasWhitespace()
        {
            string ascii = "Temp:20.5, Hum: 30.0, LED: 0.20";

            var result = PayloadParser.Parse(ascii);

            Assert.Equal(20.5, result.Temp);
            Assert.Equal(30.0, result.Hum);
            Assert.Equal(0.20, result.Led);
        }

        [Fact]
        public void Parse_ShouldThrowException_WhenInvalidFormat()
        {
            string badAscii = "InvalidPayload";

            Assert.Throws<FormatException>(() => PayloadParser.Parse(badAscii));
        }
    }
}

