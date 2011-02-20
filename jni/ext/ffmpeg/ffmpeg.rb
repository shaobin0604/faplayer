#!/usr/local/bin/ruby

def loadcf(mf)
    config = Hash.new
    f = File.open('config.mak')
    while not f.eof?
        line = f.readline
        if line.scan(/^!{0,1}(CONFIG|HAVE|ARCH)_/).size == 1
            config[line.split('=')[0]] = 'yes'
        end
    end
    f.close
    config
end

def loadmf(mf, cf)
    result = Hash.new
    f = File.open(mf)
    while not f.eof?
        line = f.readline
        # OBJS
        if (line =~ (/^OBJS\s*?(\+){0,1}=/)) ||
            (line =~ /^[A-Z].+-\$\((.+?)\)/ &&
            (not line =~ /^SKIPHEADERS/) &&
            (not line =~ /^TESTPROGS/))
            whole = ''
            while not (line =~ /^$/ || !line.include?('\\'))
                whole += line
                line = f.readline
            end
            whole += line
            whole = whole.gsub(/\n/, '')
            whole = whole.gsub(/\\/, '')
            name = whole.scan(/^(.+?)\s*?(\+){0,1}=/)[0][0]
            cond = name.scan(/\$\((.+?)\)/)
            if cond.length > 0
                cond = cond[0][0].to_s
                name.delete!("$(")
                name.delete!(")")
                name = name.gsub(/#{cond}/, cf[cond] != nil ? "yes" : "no")
            end
            if !result.has_key?(name)
                result[name] = Array.new
            end
            line = whole.split('=')[1]
            # normal
            group = line.scan(/([^\s]+?)\.o/)
                group.each { |g|
                g = g[0].to_s
                # libavcodec
                file = File.dirname(mf) + "/" + File.basename(g)
                file += ".c" if File.exist?(file + ".c")
                file += ".S" if File.exist?(file + ".S")
                result[name].push(file) if File.exist?(file)
                # libavutil
                file = File.dirname(mf) + "/" + g
                file += ".c" if File.exist?(file + ".c")
                file += ".S" if File.exist?(file + ".S")
                result[name].push(file) if File.exist?(file)
            }
            # inherited
            group = line.scan(/\$\((.+?)\)/)
            group.each { |g|
                g = g[0].to_s
                if !cf.has_key?(g) && !result.has_key?(g)
                    next
                end
                result[name] += result[g]
            }
        end
    end
    f.close
    result.each_key { |key|
        result[key].uniq!
    }
    result
end

arch = 'arm'
dirs = 'libavcodec|libavcore|libavfilter|libavformat|libavutil|libswscale'

cf = loadcf('config.mk')

all = []

dirs.split('|').each { |d|
    fn = d + '/Makefile'
    files = loadmf(fn, cf) if File.exist?(fn)
    all += files['OBJS'] if files['OBJS']
    all += files['OBJS-yes'] if files['OBJS-yes']
    fn = d + '/' + arch + '/Makefile'
    if File.exist?(fn)
        files = loadmf(fn, cf)
        all += files['OBJS'] if files['OBJS']
        all += files['OBJS-yes'] if files['OBJS-yes']
    end
}

all.uniq!
all.sort!

all.each { |f|
    print "    #{f} \\\n"
}

