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

file=open('version/Version.cpp','w')
file.write('#include"Version.h"\n')
file.write('\n')
file.write('const char *Version::versionString="{:d}.{:d}.{:d}.{:d}";\n'.format(major,minor,patch,build))
file.write('const char *Version::versionMajor="{:d}";\n'.format(major))
file.write('const char *Version::versionMinor="{:d}";\n'.format(minor))
file.write('const char *Version::versionPatch="{:d}";\n'.format(patch))
file.write('const char *Version::versionBuild="{:d}";\n'.format(build))
file.write('const char *Version::lastCommitSha1="{}";\n'.format(gitSha1))
file.close()
