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

    private uchar[]     buf = new uchar[4];
    public  uchar[]     data;

    public Context(InputStream stream)
    {
        this.stream = stream;
    }

    // header의 크기 32비트(4 bytes)
    // tag id 10-bit
    // level  10-bit
    // size   12-bit
    public bool decode_header(uchar[] buf)
    {
        // decode 헤더, 태그 ID, 크기
        header  = (buf[3] << 24) |
                  (buf[2] << 16) |
                  (buf[1] <<  8) |
                   buf[0];
        tag_id = (uint16) ( header        & 0x3ff);
        level  = (uint16) ((header >> 10) & 0x3ff);
        size   = (uint32) ((header >> 20) & 0xfff);

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

    public bool pull()
    {
        // 4바이트 읽기
        try { stream.read_all(buf, out bytes_read); }
        catch (Error e) { error("%s", e.message); }
        if (bytes_read <= 0) return false;

        // 4바이트 헤더 디코딩하기
        ret = decode_header(buf);
        if (ret == false) return false;

        // data 가져오기
        uchar[]? tmp = new uchar[size];
        try { stream.read_all(tmp, out bytes_read); }
        catch (Error e) { error("%s", e.message); }
        if (bytes_read <= 0) return false;
        data = tmp;

        return true;
    }
}
