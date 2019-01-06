package buildertests;

import static org.assertj.core.api.Assertions.assertThat;
import static org.junit.jupiter.api.Assertions.*;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.io.File;

import org.junit.jupiter.api.Test;

import builder.Builder;

public class TestCase2 {
  private static final String TARGET_PLATFORM = "arduino";
  private static final String TEST_FILE = "test_cases/Test2Textbtn";
  private static final String TEST_EXT = ".txt";
  private static final String OUTPUT_FILE = "build/test-results/Test2Textbtn";
  private static final String OUTPUT_EXT = ".ino";
  
  @Test
  public void testTextbtn() {
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
