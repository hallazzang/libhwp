/*
 * test-ghwp.vala
 *
 * Copyright (C) 2012  Hodong Kim <cogniti@gmail.com>
 * 
 * This library is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

void main(string[] args)
{
    if(args.length < 2) {
        print("Usage: test-ghwp filename\n");
        return;
    }

    GHWP.GHWPFile ghwp_file;
    File file = File.new_for_path(args[1]);

    try {
        ghwp_file = new GHWP.GHWPFile(file);
    }
    catch (Error e) {
        error("%s", e.message);
    }

    print("signature           %s\n", ghwp_file.header.signature);
    print("version             %s\n", ghwp_file.header.version.to_string("%x"));
    print("compress            %s\n", ghwp_file.header.is_compress.to_string());
    print("encrypt             %s\n", ghwp_file.header.is_encrypt.to_string());
    print("distribute          %s\n", ghwp_file.header.is_distribute.to_string());
    print("script              %s\n", ghwp_file.header.is_script.to_string());
    print("drm                 %s\n", ghwp_file.header.is_drm.to_string());
    print("xml_template        %s\n", ghwp_file.header.is_xml_template.to_string());
    print("history             %s\n", ghwp_file.header.is_history.to_string());
    print("sign                %s\n", ghwp_file.header.is_sign.to_string());
    print("certificate_encrypt %s\n", ghwp_file.header.is_certificate_encrypt.to_string());
    print("sign_spare          %s\n", ghwp_file.header.is_sign_spare.to_string());
    print("certificate_drm     %s\n", ghwp_file.header.is_certificate_drm.to_string());
    print("ccl                 %s\n", ghwp_file.header.is_ccl.to_string());

    try {
        var doc = ghwp_file.get_document();
        stdout.printf("%s\n", doc.prv_text);
        // FIXME
        Gsf.doc_meta_dump(doc.summary_info);
    }
    catch (Error e) {
        error ("%s", e.message);
    }
}
