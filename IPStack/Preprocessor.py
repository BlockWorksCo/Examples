
import re
import sys


g={}
l={}

def Run(pythonText):
    global g
    global l
    exec(compile(pythonText, '<EmbdeddedPython>','exec'),g,l)
    #return eval(pythonText)
    return str(l['t'])

def EmbeddedPython(matchobj):
    pythonText  = 't='+matchobj.group(1)
    return Run(pythonText)

sourceText  = open(sys.argv[1]).read()
macros      = re.compile('#ifdef\s+PREPROCESSOR(.*?)#endif',re.MULTILINE|re.DOTALL).findall(sourceText)[0]
sourceText  = re.sub('#ifdef\s+PREPROCESSOR[\d\D]*?#endif','',sourceText, re.DOTALL|re.MULTILINE)
sourceText  = re.sub('!(.*?)!', EmbeddedPython, sourceText)
#exec(compile(macros, sys.argv[1],'exec'), g,l)
eval(macros)
Run(macros)
print(l['Process'](sourceText))
print(l['ObjectSelect'])
