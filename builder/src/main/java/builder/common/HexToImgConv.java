/**
 *
 * The MIT License
 *
 * Copyright 2019 Paul Conti
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */
package builder.common;

import java.awt.image.BufferedImage;
import java.awt.image.DataBufferUShort;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.ShortBuffer;

import javax.swing.JOptionPane;

import builder.parser.ParserException;
import builder.parser.Token;
import builder.parser.Tokenizer;

/**
 * The Class HexToImgConv reads in a C File containing an extern storage[] 
 * of Image Data and converts it to a BufferedImage.
 * Only supports one image per file and it should have been created
 * using the UTFT tool ImgConv.exe or ImageConverter565.exe
 * While other tools can create C Image files the parser may not
 * handle them correctly.
 * 
 * @author Paul Conti
 * 
 */
public class HexToImgConv {
  /** Token id for integers. */
  public static final int INTEGER                 = 1;
  
  /** Token id for floats. */
  public static final int HEX                     = 2;
  
  /** Token id for word. */
  public static final int WORD                    = 3;
  
  /** Token id for opening square bracket. */
  public static final int OPEN_SQBRACKET          = 4;
  
  /** Token id for opening brace. */
  public static final int OPEN_BRACE              = 5;
  
  /** Token id for comma. */
  public static final int COMMA                   = 6;
  
  /** Token id for semicolon. */
  public static final int SEMICOLON               = 7;
  
  /** Token id for special characters. */
  public static final int SPECIALCHAR             = 99;

  /** The Tokenizer. */
  private Tokenizer tokenizer;

  /** the extern name of C structure. */
  String externName;
  
  /** the nHeight of image. */
  int nHeight;
  
  /** the nWidth of image. */
  int nWidth;
  
  /**
   * Instantiates a new HexToImgConv.
   */
  public HexToImgConv() {
    tokenizer = new Tokenizer();
    // Create our tokenizer for C File Image parsing
    tokenizer.defineIgnored("\\s+"); // Ignore whitespace
    tokenizer.defineIgnored("^#.*$|^\\/\\/.*$"); // Ignore comments (which start with # or //)
    tokenizer.add("0x[a-fA-F0-9]+", HexToImgConv.HEX);
    tokenizer.add("[a-zA-Z][a-zA-Z0-9_]*", HexToImgConv.WORD);
    tokenizer.add("[0-9]+", HexToImgConv.INTEGER);
    tokenizer.add("\\,", HexToImgConv.COMMA);
    tokenizer.add("\\[", HexToImgConv.OPEN_SQBRACKET);
    tokenizer.add("\\{", HexToImgConv.OPEN_BRACE);
    tokenizer.add("\\;", HexToImgConv.SEMICOLON);
    tokenizer.add(".", HexToImgConv.SPECIALCHAR);
  }
  
  /**
   * Do convert.
   *
   * @param file
   *          the file
   * @return the <code>buffered image</code> object
   */
  public BufferedImage doConvert(File file) {

    if (!file.getName().endsWith(".c")) {
      return null;
    }
      
    BufferedImage image = null;
    externName = null;
    int size = 0;
    nHeight = 0;
    nWidth = 0;
    Token token = null;
    try {
      FileReader fr = null;
      fr = new FileReader(file);
      tokenizer.setSource(fr);
      
      // loop until we find we find 'const'
      while ((token = tokenizer.nextToken()).getToken() != null) {
        if (token.getType() == HexToImgConv.WORD &&
            token.getToken().equals("const"))
          break;
      }
      // pull out unsigned short
      token = tokenizer.nextToken();
      if (token.getType() != HexToImgConv.WORD || 
          !token.getToken().equals("unsigned") ) parseError(token, "unsigned");
      token = tokenizer.nextToken();
      if (token.getType() != HexToImgConv.WORD || 
          !token.getToken().equals("short") ) parseError(token, "short");
      // hopefully Token is our extern storage name
      token = tokenizer.nextToken();
      if (token.getType() != HexToImgConv.WORD) parseError(token, "storage_name[]");
      externName = token.getToken();
      // now figure out byte array size
      token = tokenizer.nextToken();
      if (token.getType() != HexToImgConv.OPEN_SQBRACKET) parseError(token, "[size of array]");
      token = tokenizer.nextToken();
      if (token.getType() == HexToImgConv.INTEGER) {
        size = Integer.valueOf(token.getToken());
        token = tokenizer.nextToken();
        if (token.getToken().equals("+"))
          size = size+2;
      } else if (token.getType() == HexToImgConv.HEX) {
        size = Integer.decode(token.getToken()).intValue();
        token = tokenizer.nextToken();
        if (token.getToken().equals("+"))
          size = size+2;
      } else {
        parseError(token, "[size of array]");
      }
      // skip pass until we get '{'
      while ((token = tokenizer.nextToken()).getType() != HexToImgConv.OPEN_BRACE) { }
      // grab nHeight and nWidth next
      token = tokenizer.nextToken();
      if (token.getType() != HexToImgConv.INTEGER) parseError(token, "image nHeight");
      nHeight = Integer.valueOf(token.getToken());
      token = tokenizer.nextToken();
      if (token.getType() != HexToImgConv.COMMA) parseError(token, ",");
      token = tokenizer.nextToken();
      if (token.getType() != HexToImgConv.INTEGER) parseError(token, "image nWidth");
      nWidth = Integer.valueOf(token.getToken());
      // now build up our image inside a byte array
      int i = 0;
      int nRGB;
      byte[] bytes = new byte[(size-1)*2];
      byte hiByte, lowByte;
      while ((token = tokenizer.nextToken()).getType() != HexToImgConv.SEMICOLON) {
        if (token.getType() == HexToImgConv.HEX) {
          nRGB = Integer.decode(token.getToken());
          hiByte = (byte) ((nRGB & 0xFF00) >> 8);
          bytes[i++] = hiByte;
          lowByte = (byte)(nRGB & 0xFF);
          bytes[i++] = lowByte;
        }
      }
      fr.close();
      // now we need to convert our byte array to unsigned shorts
      // NOTE: Why not simply use the decoded nRGB which is really a 16-bit value? 
      //       Because Java doesn't support unsigned ints, or shorts.
      //       short nRGB would give a number size exception on the decode() on
      //       finding the first number larger than 32767. (don't ask how I know).
      ShortBuffer byteBuffer = ByteBuffer.wrap(bytes)
          .order(ByteOrder.BIG_ENDIAN) // Or LITTLE_ENDIAN depending on the spec of the card
          .asShortBuffer();            // Our data will be 16 bit unsigned shorts
      // Create a buffered image 
      image = new BufferedImage(nWidth, nHeight, BufferedImage.TYPE_USHORT_565_RGB);
      // Cast our input data to unsigned short, of course, Java doesn't make this easy
      // so we use the class (DataBufferUShort) which we don't even use except for the cast. 
      short[] data = ((DataBufferUShort) image.getRaster().getDataBuffer()).getData();
      byteBuffer.get(data);
    } catch (IOException | ParserException | NumberFormatException e) {
      String msg = String.format("File '%s'\n'%s'\n", 
          file.getName(), e.toString());
      JOptionPane.showMessageDialog(null, msg, "Error", JOptionPane.ERROR_MESSAGE);
    }
    return image;
  }
 
  /**
   * Gets the extern name.
   *
   * @return the extern name
   */
  public String getExternName() {
    return externName;
  }
  
  /**
   * Gets the width.
   *
   * @return the width
   */
  public int getWidth() {
    return nWidth;
  }
  
  /**
   * Gets the height.
   *
   * @return the height
   */
  public int getHeight() {
    return nHeight;
  }
  
  /**
   * Parses the error.
   *
   * @param t
   *          the t
   * @param s
   *          the s
   * @throws ParserException
   *           the parser exception
   */
  public void parseError(Token t, String s) throws ParserException {
    throw new ParserException("Missing '" + s + "' found <"+ t.getToken() +"> line: " + t.getLineNumber());
  }

}
