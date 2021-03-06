""" Python Character Mapping Codec generated from 'CP864.TXT' with gencodec.py.

Written by Marc-Andre Lemburg (mal@lemburg.com).

(c) Copyright CNRI, All Rights Reserved. NO WARRANTY.
(c) Copyright 2000 Guido van Rossum.

"""#"

import codecs

### Codec APIs

class Codec(codecs.Codec):

    def encode(self,input,errors='strict'):

        return codecs.charmap_encode(input,errors,encoding_map)

    def decode(self,input,errors='strict'):

        return codecs.charmap_decode(input,errors,decoding_map)

class StreamWriter(Codec,codecs.StreamWriter):
    pass

class StreamReader(Codec,codecs.StreamReader):
    pass

### encodings module API

def getregentry():

    return (Codec().encode,Codec().decode,StreamReader,StreamWriter)

### Decoding Map

decoding_map = codecs.make_identity_dict(range(256))
decoding_map.update({
        0x0025: 0x066a, # ARABIC PERCENT SIGN
        0x0080: 0x00b0, # DEGREE SIGN
        0x0081: 0x00b7, # MIDDLE DOT
        0x0082: 0x2219, # BULLET OPERATOR
        0x0083: 0x221a, # SQUARE ROOT
        0x0084: 0x2592, # MEDIUM SHADE
        0x0085: 0x2500, # FORMS LIGHT HORIZONTAL
        0x0086: 0x2502, # FORMS LIGHT VERTICAL
        0x0087: 0x253c, # FORMS LIGHT VERTICAL AND HORIZONTAL
        0x0088: 0x2524, # FORMS LIGHT VERTICAL AND LEFT
        0x0089: 0x252c, # FORMS LIGHT DOWN AND HORIZONTAL
        0x008a: 0x251c, # FORMS LIGHT VERTICAL AND RIGHT
        0x008b: 0x2534, # FORMS LIGHT UP AND HORIZONTAL
        0x008c: 0x2510, # FORMS LIGHT DOWN AND LEFT
        0x008d: 0x250c, # FORMS LIGHT DOWN AND RIGHT
        0x008e: 0x2514, # FORMS LIGHT UP AND RIGHT
        0x008f: 0x2518, # FORMS LIGHT UP AND LEFT
        0x0090: 0x03b2, # GREEK SMALL BETA
        0x0091: 0x221e, # INFINITY
        0x0092: 0x03c6, # GREEK SMALL PHI
        0x0093: 0x00b1, # PLUS-OR-MINUS SIGN
        0x0094: 0x00bd, # FRACTION 1/2
        0x0095: 0x00bc, # FRACTION 1/4
        0x0096: 0x2248, # ALMOST EQUAL TO
        0x0097: 0x00ab, # LEFT POINTING GUILLEMET
        0x0098: 0x00bb, # RIGHT POINTING GUILLEMET
        0x0099: 0xfef7, # ARABIC LIGATURE LAM WITH ALEF WITH HAMZA ABOVE ISOLATED FORM
        0x009a: 0xfef8, # ARABIC LIGATURE LAM WITH ALEF WITH HAMZA ABOVE FINAL FORM
        0x009b: None,   # UNDEFINED
        0x009c: None,   # UNDEFINED
        0x009d: 0xfefb, # ARABIC LIGATURE LAM WITH ALEF ISOLATED FORM
        0x009e: 0xfefc, # ARABIC LIGATURE LAM WITH ALEF FINAL FORM
        0x009f: None,   # UNDEFINED
        0x00a1: 0x00ad, # SOFT HYPHEN
        0x00a2: 0xfe82, # ARABIC LETTER ALEF WITH MADDA ABOVE FINAL FORM
        0x00a5: 0xfe84, # ARABIC LETTER ALEF WITH HAMZA ABOVE FINAL FORM
        0x00a6: None,   # UNDEFINED
        0x00a7: None,   # UNDEFINED
        0x00a8: 0xfe8e, # ARABIC LETTER ALEF FINAL FORM
        0x00a9: 0xfe8f, # ARABIC LETTER BEH ISOLATED FORM
        0x00aa: 0xfe95, # ARABIC LETTER TEH ISOLATED FORM
        0x00ab: 0xfe99, # ARABIC LETTER THEH ISOLATED FORM
        0x00ac: 0x060c, # ARABIC COMMA
        0x00ad: 0xfe9d, # ARABIC LETTER JEEM ISOLATED FORM
        0x00ae: 0xfea1, # ARABIC LETTER HAH ISOLATED FORM
        0x00af: 0xfea5, # ARABIC LETTER KHAH ISOLATED FORM
        0x00b0: 0x0660, # ARABIC-INDIC DIGIT ZERO
        0x00b1: 0x0661, # ARABIC-INDIC DIGIT ONE
        0x00b2: 0x0662, # ARABIC-INDIC DIGIT TWO
        0x00b3: 0x0663, # ARABIC-INDIC DIGIT THREE
        0x00b4: 0x0664, # ARABIC-INDIC DIGIT FOUR
        0x00b5: 0x0665, # ARABIC-INDIC DIGIT FIVE
        0x00b6: 0x0666, # ARABIC-INDIC DIGIT SIX
        0x00b7: 0x0667, # ARABIC-INDIC DIGIT SEVEN
        0x00b8: 0x0668, # ARABIC-INDIC DIGIT EIGHT
        0x00b9: 0x0669, # ARABIC-INDIC DIGIT NINE
        0x00ba: 0xfed1, # ARABIC LETTER FEH ISOLATED FORM
        0x00bb: 0x061b, # ARABIC SEMICOLON
        0x00bc: 0xfeb1, # ARABIC LETTER SEEN ISOLATED FORM
        0x00bd: 0xfeb5, # ARABIC LETTER SHEEN ISOLATED FORM
        0x00be: 0xfeb9, # ARABIC LETTER SAD ISOLATED FORM
        0x00bf: 0x061f, # ARABIC QUESTION MARK
        0x00c0: 0x00a2, # CENT SIGN
        0x00c1: 0xfe80, # ARABIC LETTER HAMZA ISOLATED FORM
        0x00c2: 0xfe81, # ARABIC LETTER ALEF WITH MADDA ABOVE ISOLATED FORM
        0x00c3: 0xfe83, # ARABIC LETTER ALEF WITH HAMZA ABOVE ISOLATED FORM
        0x00c4: 0xfe85, # ARABIC LETTER WAW WITH HAMZA ABOVE ISOLATED FORM
        0x00c5: 0xfeca, # ARABIC LETTER AIN FINAL FORM
        0x00c6: 0xfe8b, # ARABIC LETTER YEH WITH HAMZA ABOVE INITIAL FORM
        0x00c7: 0xfe8d, # ARABIC LETTER ALEF ISOLATED FORM
        0x00c8: 0xfe91, # ARABIC LETTER BEH INITIAL FORM
        0x00c9: 0xfe93, # ARABIC LETTER TEH MARBUTA ISOLATED FORM
        0x00ca: 0xfe97, # ARABIC LETTER TEH INITIAL FORM
        0x00cb: 0xfe9b, # ARABIC LETTER THEH INITIAL FORM
        0x00cc: 0xfe9f, # ARABIC LETTER JEEM INITIAL FORM
        0x00cd: 0xfea3, # ARABIC LETTER HAH INITIAL FORM
        0x00ce: 0xfea7, # ARABIC LETTER KHAH INITIAL FORM
        0x00cf: 0xfea9, # ARABIC LETTER DAL ISOLATED FORM
        0x00d0: 0xfeab, # ARABIC LETTER THAL ISOLATED FORM
        0x00d1: 0xfead, # ARABIC LETTER REH ISOLATED FORM
        0x00d2: 0xfeaf, # ARABIC LETTER ZAIN ISOLATED FORM
        0x00d3: 0xfeb3, # ARABIC LETTER SEEN INITIAL FORM
        0x00d4: 0xfeb7, # ARABIC LETTER SHEEN INITIAL FORM
        0x00d5: 0xfebb, # ARABIC LETTER SAD INITIAL FORM
        0x00d6: 0xfebf, # ARABIC LETTER DAD INITIAL FORM
        0x00d7: 0xfec1, # ARABIC LETTER TAH ISOLATED FORM
        0x00d8: 0xfec5, # ARABIC LETTER ZAH ISOLATED FORM
        0x00d9: 0xfecb, # ARABIC LETTER AIN INITIAL FORM
        0x00da: 0xfecf, # ARABIC LETTER GHAIN INITIAL FORM
        0x00db: 0x00a6, # BROKEN VERTICAL BAR
        0x00dc: 0x00ac, # NOT SIGN
        0x00dd: 0x00f7, # DIVISION SIGN
        0x00de: 0x00d7, # MULTIPLICATION SIGN
        0x00df: 0xfec9, # ARABIC LETTER AIN ISOLATED FORM
        0x00e0: 0x0640, # ARABIC TATWEEL
        0x00e1: 0xfed3, # ARABIC LETTER FEH INITIAL FORM
        0x00e2: 0xfed7, # ARABIC LETTER QAF INITIAL FORM
        0x00e3: 0xfedb, # ARABIC LETTER KAF INITIAL FORM
        0x00e4: 0xfedf, # ARABIC LETTER LAM INITIAL FORM
        0x00e5: 0xfee3, # ARABIC LETTER MEEM INITIAL FORM
        0x00e6: 0xfee7, # ARABIC LETTER NOON INITIAL FORM
        0x00e7: 0xfeeb, # ARABIC LETTER HEH INITIAL FORM
        0x00e8: 0xfeed, # ARABIC LETTER WAW ISOLATED FORM
        0x00e9: 0xfeef, # ARABIC LETTER ALEF MAKSURA ISOLATED FORM
        0x00ea: 0xfef3, # ARABIC LETTER YEH INITIAL FORM
        0x00eb: 0xfebd, # ARABIC LETTER DAD ISOLATED FORM
        0x00ec: 0xfecc, # ARABIC LETTER AIN MEDIAL FORM
        0x00ed: 0xfece, # ARABIC LETTER GHAIN FINAL FORM
        0x00ee: 0xfecd, # ARABIC LETTER GHAIN ISOLATED FORM
        0x00ef: 0xfee1, # ARABIC LETTER MEEM ISOLATED FORM
        0x00f0: 0xfe7d, # ARABIC SHADDA MEDIAL FORM
        0x00f1: 0x0651, # ARABIC SHADDAH
        0x00f2: 0xfee5, # ARABIC LETTER NOON ISOLATED FORM
        0x00f3: 0xfee9, # ARABIC LETTER HEH ISOLATED FORM
        0x00f4: 0xfeec, # ARABIC LETTER HEH MEDIAL FORM
        0x00f5: 0xfef0, # ARABIC LETTER ALEF MAKSURA FINAL FORM
        0x00f6: 0xfef2, # ARABIC LETTER YEH FINAL FORM
        0x00f7: 0xfed0, # ARABIC LETTER GHAIN MEDIAL FORM
        0x00f8: 0xfed5, # ARABIC LETTER QAF ISOLATED FORM
        0x00f9: 0xfef5, # ARABIC LIGATURE LAM WITH ALEF WITH MADDA ABOVE ISOLATED FORM
        0x00fa: 0xfef6, # ARABIC LIGATURE LAM WITH ALEF WITH MADDA ABOVE FINAL FORM
        0x00fb: 0xfedd, # ARABIC LETTER LAM ISOLATED FORM
        0x00fc: 0xfed9, # ARABIC LETTER KAF ISOLATED FORM
        0x00fd: 0xfef1, # ARABIC LETTER YEH ISOLATED FORM
        0x00fe: 0x25a0, # BLACK SQUARE
        0x00ff: None,   # UNDEFINED
})

### Encoding Map

encoding_map = codecs.make_encoding_map(decoding_map)
