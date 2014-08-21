
import re
import sys


def EmbeddedPython(matchobj):
    pythonText  = matchobj.group(1)
    #return exec(compile(pythonText, '<EmbdeddedPython>','exec'))
    return eval(pythonText)

sourceText  = open(sys.argv[1]).read()
macros      = re.compile('#ifdef\s+PREPROCESSOR(.*?)#endif',re.MULTILINE|re.DOTALL).findall(sourceText)[0]
sourceText  = re.sub('#ifdef\s+PREPROCESSOR[\d\D]*?#endif','',sourceText, re.DOTALL|re.MULTILINE)
sourceText  = re.sub('!(.*?)!', EmbeddedPython, sourceText)
#print('--------------------------------')
#print(sourceText)
#print('--------------------------------')
exec(compile(macros, sys.argv[1],'exec'))
#print('--------------------------------')
print(Process(sourceText))
#print('--------------------------------')
