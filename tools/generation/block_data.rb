require "yaml"

input_path = ARGV.fetch(0)

config = YAML.safe_load(
    File.read(input_path),
    permitted_classes: [],
    permitted_symbols: [],
    aliases: false,
    filename: input_path
)

blocks = config.fetch("blocks")