
import re
import sys


g={}
l={}

def Run(pythonText):
    global g
    global l
    exec(compile(pythonText, '<EmbeddedPython>','exec'),g,l)
    return str(l['t'])

def ReplaceEmbeddedPython(matchobj):
    pythonText  = 't='+matchobj.group(1)
    return Run(pythonText)

sourceText  = open(sys.argv[1]).read()

macros      = re.compile('#ifdef\s+PREPROCESSOR(.*?)#endif',re.MULTILINE|re.DOTALL).findall(sourceText)[0]
sourceText  = re.sub('#ifdef\s+PREPROCESSOR[\d\D]*?#endif','#define PREPROCESSED',sourceText, re.DOTALL|re.MULTILINE)
Run("\nt=''\n"+macros)

sourceText  = re.sub('!(.*?)!', ReplaceEmbeddedPython, sourceText)
print(sourceText)
