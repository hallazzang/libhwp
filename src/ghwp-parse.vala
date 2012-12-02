public class GHWP.Context : Object
{
    private InputStream stream;
    // record header
    private uint32      header;
    public  uint16      tag_id;
    public  uint16      level;
    public  uint32      size;

    private size_t      bytes_read;
    private bool        ret;

    public  uchar[]     buf = new uchar[4];
    public  uchar[]     data;

    public Context(InputStream stream)
    {
        this.stream = stream;
    }

    // header의 크기 32비트(4 bytes)
    // tag id 10-bit
    // level  10-bit
    // size   12-bit
    public bool parse_header(InputStream stream)
    {
        try { stream.read_all(buf, out bytes_read); }
        catch (Error e) { error("%s", e.message); }

        if (bytes_read <= 0) return false;

        // decode 헤더, 태그 ID, 크기
        header  = (buf[3] << 24) |
                  (buf[2] << 16) |
                  (buf[1] <<  8) |
                   buf[0];
        tag_id = (uint16) ( header        & 0x3ff);
        level  = (uint16) ((header >> 10) & 0x3ff);
        size   = (uint32) ((header >> 20) & 0xfff);
        //stdout.printf("%ld %d %d %ld\n", header, tag_id, level, size);
        //stdout.printf("%s lv %d sz %d\n", GHWP.Tag.NAMES[tag_id], level, (int)size);
        if (size == 0xfff) {
            try { stream.read_all(buf, out bytes_read); }
            catch (Error e) { error("%s", e.message); }

            if (bytes_read <= 0) return false;

            size  = (buf[3] << 24) |
                    (buf[2] << 16) |
                    (buf[1] <<  8) |
                     buf[0];
        }
        return true;
    }

    public void parse()
    {
        do {
            ret = parse_header(stream);

            if (ret == false || size == 0) break;

            uchar[]? tmp = new uchar[size];
            try { stream.read_all(tmp, out bytes_read); }
            catch (Error e) { error("%s", e.message); }
            data = tmp;

            // 2바이트씩 묶어서 처리.
            unichar ch;
            var str = new StringBuilder("");
            if (tag_id == GHWP.Tag.PARA_TEXT) {
                for (int i = 0; i < tmp.length; i = i + 2) {
                    str.erase(0);
                    ch = (tmp[i+1] <<  8) | tmp[i];
                    str.append_unichar(ch);
                    switch (ch) {
                    case 0:
                        break;
                    case 10:
                    case 13:
                        stdout.printf("%c", 10);
                        break;
                    case 24:
                    case 25:
                    case 26:
                    case 27:
                    case 28:
                    case 29:
                    case 31:
                        break;
                    case 30:
                        stdout.printf("%c", 0x20);
                        break;
                    case 4:
                    case 5:
                    case 6:
                    case 7:
                    case 8:
                    case 19:
                    case 20:
                        i += 14;
                        break;
                    case 9:
                        stdout.printf("%c", 9);
                        i += 14;
                        break;
                    case 1:
                    case 2:
                    case 3:
                    case 11:
                    case 12:
                    case 14:
                    case 15:
                    case 16:
                    case 17:
                    case 18:
                    case 21:
                    case 22:
                    case 23:
                        i += 14;
                        break;
                    default:
                        stdout.printf("%s", str.str);
                        break;
                    }
                }
            }
        } while (bytes_read > 0);
    }
}
