#!/usr/local/bin/ruby

all = Hash.new
list = `find . -name Android.mk`.split("\n")
list.each { |l|
    next if ((l =~ /^\.\/jni\/vlc\/modules/) == nil)
    temp = l.scan(/modules\/([^\/]+)\//)
    next if temp == nil || temp.size == 0
    t = temp[0][0].to_s
    all[t] = Array.new if !all.has_key?(t)
    File.open(l) { |f|
        while !f.eof?
            ln = f.readline
            next if ((ln =~ /^LOCAL_MODULE/) == nil)
            temp = ln.scan(/\s*LOCAL_MODULE\s*:=\s*([^\s]+)/)
            next if temp == nil || temp.size == 0
            n = 'lib' + temp[0][0].to_s + '.so'
            all[t].push(n)
        end
    }
}
all.each { |k, a|
    `mkdir -p assets/lib/#{k}`
    a.each { |v|
        `mv libs/armeabi/#{v} assets/lib/#{k}`
    }
}
`rm -f assets/index.txt`
list = `cd assets && find . -type f`.split("\n")
File.open('assets/index.txt', 'w') { |f|
   list.each { |l|
        f.write(l[2..-1] + "\n")
    }
}
