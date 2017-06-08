s=proc{|f,g,x|f[x][g[x]]}.curry
k=proc{|x,y|x}.curry
i=proc{|x|x}.curry
fi0=[]
re0=proc{|x|fi0.size;x}.curry
[s[s[i][i]][k[i]]][0][s[s[k[s]][s[k[s]][s[s[k[s]][s[k[s[k[re0]]]][s[k[s]][k]]]][k]]]][k[s[k[s]][k]]]]
