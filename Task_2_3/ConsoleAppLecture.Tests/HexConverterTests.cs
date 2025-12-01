using ConsoleAppLecture.Helpers;
using Xunit;

namespace ConsoleAppLecture.Tests
{
    public class HexConverterTests
    {
        [Fact]
        public void HexToAscii_ShouldConvertValidValues_WhenDecodingInput()
        {
            // Arrange
            string hex = "54-65-73-74"; // Test

            // Act
            string result = HexConverter.HexToAscii(hex);

            // Assert
            Assert.Equal("Test", result);
        }

        [Fact]
        public void HexToAscii_ShouldThrowException_WhenUsingBadInput()
        {
            // Arrange
            string hex = "ZZ-11-22";  // invalid hex

            // Act + Assert
            Assert.Throws<FormatException>(() => HexConverter.HexToAscii(hex));
        }

        [Fact]
        public void HexToAscii_ShouldThrowException_WhenOddLength()
        {
            string hex = "ABC";  // odd length

            Assert.Throws<ArgumentException>(() => HexConverter.HexToAscii(hex));
        }
    }
}
