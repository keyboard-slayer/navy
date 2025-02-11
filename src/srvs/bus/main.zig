const std = @import("std");
const bus = @import("./lib.zig");
const log = std.log.scoped(.bus);
const read = @import("navy").read;

const BROADCAST = 1;

pub fn main() !void {
    var buffer: [64]u8 = undefined;

    log.info("Hello from bus", .{});

    while (true) {
        const capId = read(BROADCAST, &buffer, 64);
        const req = bus.Ipc.from_buffer(buffer);

        switch (req.req_type) {
            .helo => {
                log.info("Got HELO: {s} from {x}", .{ req.helo.name, 1 | capId << 16 });
                _ = (bus.Ack{}).send_raw(1 | capId << 16);
            },
            .ack => {},
        }
    }
}
