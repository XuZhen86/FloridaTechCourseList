import subprocess

file=open('version/Version.txt','r')
major,minor,patch,build=[int(val) for val in file.readlines()]
file.close()

build+=1
print('Version {:d}.{:d}.{:d}.{:d}'.format(major,minor,patch,build))

file=open('version/Version.txt','w')
file.write('{:d}\n{:d}\n{:d}\n{:d}\n'.format(major,minor,patch,build))
file.close()

gitSha1=subprocess.run(['git','rev-parse','HEAD'],stdout=subprocess.PIPE).stdout.decode('utf-8')[:-1]

file=open('version/Version.h','w')
file.write('#ifndef VERSION_H\n')
file.write('#define VERSION_H\n')
file.write('\n')
file.write('#include<QString>\n')
file.write('\n')
file.write('static const QString versionString="{:d}.{:d}.{:d}.{:d}";\n'.format(major,minor,patch,build))
file.write('\n')
file.write('static const QString versionMajor="{:d}";\n'.format(major))
file.write('static const QString versionMinor="{:d}";\n'.format(minor))
file.write('static const QString versionPatch="{:d}";\n'.format(patch))
file.write('static const QString versionBuild="{:d}";\n'.format(build))
file.write('\n')
file.write('static const QString lastCommitSha1="{}";\n'.format(gitSha1))
file.write('\n')
file.write('#endif')
file.write('\n')
file.close()
