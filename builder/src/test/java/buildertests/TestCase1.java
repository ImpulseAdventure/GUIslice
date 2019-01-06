package buildertests;
import org.junit.jupiter.api.Test;
import static org.assertj.core.api.Assertions.*;
import static org.junit.jupiter.api.Assertions.assertTrue;

import builder.Builder;

//import static org.junit.jupiter.api.Assertions.fail;

import java.io.File;
 
public class TestCase1 {
  private static final String TARGET_PLATFORM = "arduino";
  private static final String TEST_FILE = "test_cases/Test1Radiobtn";
  private static final String TEST_EXT = ".txt";
  private static final String OUTPUT_FILE = "build/test-results/Test1Radiobtn";
  private static final String OUTPUT_EXT = ".ino";
  
  @Test
  public void testRadiobtn() {
    Builder classUnderTest = new Builder();
    assertTrue(classUnderTest.testCodeGen(TARGET_PLATFORM, TEST_FILE, OUTPUT_FILE), "testCodeGen should return 'true'");
    File outputFile = new File(OUTPUT_FILE + OUTPUT_EXT);
    File expectedFile = new File(TEST_FILE + TEST_EXT);
    assertThat(outputFile).hasSameContentAs(expectedFile);
    if (outputFile.exists()) {
      outputFile.delete();
    }
  }
}
