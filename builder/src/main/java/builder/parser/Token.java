/**
 *
 * The MIT License
 *
 * Copyright 2018, 2019 Paul Conti
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
package builder.parser;

/**
 * A token that is produced by Tokenizer and fed into Parser.parse
 * 
 * A token consists of a token identifier, a string that the token was
 * created from and the line number in the input file. 
 * 
 * The token id must be unique among tokens and not 0 as that is reserved for EOF.
 * 
 * @author Paul Conti
 * 
 */
public class Token {
  
  /** the token identifier. */
  private int type;
  
  /** the token string. */
  private String token;
  
  /** the lineNumber of the token in the input file. */
  private int lineNumber;

  /**
   * Construct the token with its values.
   *
   * @param type
   *          the token identifier
   * @param token
   *          the string that the token was created from
   * @param lineNumber
   *          of the token in the input file
   */
  public Token(int type, String token, int lineNumber) {
    this.type = type;
    this.token = token;
    this.lineNumber = lineNumber;
  }

  /**
   * Gets the type.
   *
   * @return the type
   */
  public int getType() {
    return type;
  }
  
  /**
   * Gets the token.
   *
   * @return the token
   */
  public String getToken() {
    return token;
  }
  
  /**
   * Gets the line number.
   *
   * @return the line number
   */
  public int getLineNumber() {
    return lineNumber;
  }
}