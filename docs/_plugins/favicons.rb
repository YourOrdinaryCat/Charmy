#!/usr/bin/env ruby
require 'fileutils'

module Jekyll
  Jekyll::Hooks.register :site, :post_write do |post|
    Dir.glob('_favicons/*.*') do |file|
      sourcePath = file
      outputPath = File.join('_site', File.basename(file) )
      FileUtils.cp(sourcePath, outputPath)
    end
  end
end
