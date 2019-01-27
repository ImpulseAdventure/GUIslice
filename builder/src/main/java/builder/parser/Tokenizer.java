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

import java.util.regex.Pattern;
import java.util.regex.Matcher;

import java.util.List;
import java.util.ArrayList;
import java.util.Set;
import java.util.HashSet;
import java.io.BufferedReader;
import java.io.Reader;
import java.io.StringReader;
import java.io.IOException;

/**
 * Tokenizer is a class for reading an input file or (string for testing) and
 * separating it into tokens that can be fed into a Parser.
 * <p>
 * The user can add regular expressions that will be matched against the front
 * of the lines. Method defineKeywords() lets you declare some tokens to be
 * keywords.
 * <p/>
 * <p>
 * Methods addToken() and defineIgnored() are used to determine what the scanner
 * considers to be a token and what should be ignored (usually you want to
 * ignore whitespace and comments).
 * <p/>
 * <p>
 * Tokens are defined by calling addToken() passing in the regex pattern to scan
 * and the 'type' of token used as an identifier. The token will be stored as a
 * instance of the Internal class of TokenInfo.
 * <p/>
 * <p>
 * TokenInfo stores the token regex pattern and the registered 'type' of token.
 * A 'type' must always a unique 'int' passed in as the second parameter of
 * addToken().
 * <p/>
 * <p>
 * The method that reports the current token is nextToken(). It returns an
 * instance of the class Token. The class Token has methods to report the
 * current token [getToken()], the current 'type' [getType()], and the
 * lineNumber [getLineNumber()]. The token itself is a String, exactly as the
 * token appears in the input file or input string. The EOF token is reported as
 * a null with token 'type' of 0.
 * <p/>
 * <p>
 * Calling nextToken() will change the current token. Method advance() is called
 * internally by nextToken() to pass over the current token and go on to the
 * next one.
 * <p/>
 * <p>
 * The source for tokens is determined by setSource(). It can use either a
 * Reader or a String. The String source is useful for debugging.
 * <p/>
 * 
 * @author Paul Conti
 * 
 */
public class Tokenizer {
  /**
   * Internal class holding the information about a token type.
   */
  private class TokenInfo {
    
    /** the regular expression to match against. */
    public Pattern patternString;
    
    /** the token id that the regular expression is linked to. */
    public int type;

    /**
     * Construct TokenInfo with its values.
     *
     * @param patternString
     *          the pattern string
     * @param type
     *          the type
     */
    public TokenInfo(Pattern patternString, int type) {
      super();
      this.patternString = patternString;
      this.type = type;
    }
  }

  /**
   * a list of TokenInfo objects Each token type corresponds to one entry in the
   * list.
   */
  private List<TokenInfo> tokenList = new ArrayList<TokenInfo>();

  /** The keywords. */
  private Set<String> keywords; // Keyword tokens
  
  /** The ignoring string. */
  private String ignoringString; // Pattern string of stuff to ignore
  
  /** The ignoring. */
  private Pattern ignoring; // Pattern of stuff to ignore
  
  /** The in file. */
  private BufferedReader inFile; // Input file
  
  /** The line count. */
  private int lineCount; // Current line number in file
  
  /** The line. */
  private String line; // Current line
  
  /** The token. */
  private Token token; // Current token
  
  /** The keyword type. */
  private int keywordType; // keyword type
  
  /** The b repeat token. */
  private boolean bRepeatToken;
  
  /**
   * Default constructor.
   */
  public Tokenizer() {
    keywords = new HashSet<String>();
    ignoringString = "";
    ignoring = Pattern.compile(ignoringString);
    keywordType = -1;
  }

  /**
   * Specify the file to be used as token source.
   * 
   * @param file
   *          the input file
   */
  public void setSource(Reader file) {
    inFile = new BufferedReader(file);
    token = new Token(-1, null, -1);
    lineCount = 0;
    line = "";
    bRepeatToken=false;
  }

  /**
   * Specify a String to use as token source. Useful for testing and debugging.
   * 
   * @param string
   *          the input string
   */
  public void setSource(String string) {
    inFile = new BufferedReader(new StringReader(string));
    token = new Token(-1, null, -1);
    lineCount = 0;
    line = "";
    bRepeatToken=false;
  }

  /**
   * get the current line. Useful for testing and debugging.
   * 
   * @return line
   *          the current line string
   */
  public String getLine() {
    return line;
  }

  /**
   * Define a new pattern to be ignored. Ignored patterns are typically those for
   * white-space and comments.
   * 
   * @param patternString
   *          a pattern string as defined in class Pattern.
   */
  public void defineIgnored(String patternString) {
    if (ignoringString.length() != 0)
      ignoringString += "|";
    ignoringString += "(" + patternString + ")";
    ignoring = Pattern.compile(ignoringString);
  }

  /**
   * Add a regular expression and a type id to the internal list of recognized
   * tokens Note that order is important. For instance, if the pattern for an
   * integer is entered before the pattern for a floating point number then a
   * number such as 15.3 will be interpreted as "15" followed by ".3". The type of
   * 0 is used for EOF.
   * 
   * @param patternString
   *          the regular expression to match against
   * @param type
   *          the token id of the regular expression
   */
  public void add(String patternString, int type) {
    if (type == 0)
      throw new TokenizerException("Error: type 0 in use.");
    try {
      tokenList.add(new TokenInfo(Pattern.compile(patternString), type));
    } catch (java.util.regex.PatternSyntaxException e) {
      throw new TokenizerException("PatternSyntax: " + e.toString());
    }
  }

  /**
   * Define keywords.
   * 
   * @param type
   *          the token id of keywords
   * @param keywords
   *          an arbitrary number of keywords
   */
  public void defineKeywords(int type, String... keywords) {
    keywordType = type;
    for (String k : keywords)
      this.keywords.add(k);
  }

  /**
   * Find the given pattern at the front of the text.
   * 
   * @param pattern
   *          the pattern to look for
   * @param text
   *          the text to look in
   * @return the length of the matching portion of the text (0 if no match)
   */
  private static int find(Pattern pattern, String text) {
    Matcher m = pattern.matcher(text);
    if (m.lookingAt())
      return m.end();
    return 0;
  }

  /**
   * Advance by one token.
   * 
   * @throws TokenizerException
   *           if the source cannot be read or if non-token in source (token def
   *           is bad)
   */
  private void advance() throws TokenizerException {
    while (true) {
      // Get a new line of input, if necessary
      if (line.length() == 0) {
        try {
          line = inFile.readLine();
          if (line == null) { // EOF reached
            token = new Token(0, null, lineCount);
            inFile.close();
            return;
          }
          lineCount++;
        } catch (IOException e) {
          throw new TokenizerException("Cannot read source file: " + e.toString());
        }
        continue;
      }
      // Check for stuff that is supposed to be ignored
      int index = find(ignoring, line);
      if (index > 0) {
        line = line.substring(index);
        continue;
      }
      // Check for a token
      int type = 0;
      String tokenStr = null;
      for (TokenInfo info : tokenList) {
        Pattern pattern = info.patternString;
        index = find(pattern, line);
        if (index == 0)
          continue;
        tokenStr = line.substring(0, index);
        line = line.substring(index);
        if (keywords.contains(tokenStr))
          type = keywordType;
        else
          type = info.type;
        token = new Token(type, tokenStr, lineCount);
        return;
      }
      // Can only reach this part if the token definitions are bad
      throw new TokenizerException("No token found: " + line + "\nConsider adding '.' as a token");
    }
  }

  /**
   * Get the current token.
   *
   * @return a Token object
   */
  public Token nextToken() {
    try {
      // don't go pass EOF
      if (token.getType() != 0) {
        if (!bRepeatToken) {
          advance();
        }
        bRepeatToken=false;
      } else {
        throw new TokenizerException("read pass EOF");
      }
    } catch(TokenizerException e) {
      e.printStackTrace();
    }
    return token;
  }
  
  /**
   * Push back token used when we need to look ahead one token.
   */
  public void pushToken() {
    bRepeatToken=true;
  }
}
