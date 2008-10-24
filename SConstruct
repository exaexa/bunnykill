
env=Environment()
env['CC']='g++'
env['CCFLAGS']='-O2'
env['LIBS']=['GL','GLU','SDL','openal','alut','vorbisfile','freetype','png']
env['CPPPATH']=['#include/','#exabase/']
env.SetOption("num_jobs",2);

Export('env')

objs=[]

o=SConscript("src/SConscript");
objs+=o;
SConscript("exabase/SConscript");
for i in ['base','font','particles','sound','image']:
	objs.append('exabase/exa-'+i+'.o');

env.Program('bunnykill',objs)
env.AddPostAction('bunnykill','strip bunnykill')

