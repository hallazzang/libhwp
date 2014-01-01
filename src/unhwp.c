/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * unhwp.c
 * 
 * Copyright (C) 2013 Hodong Kim <hodong@cogno.org>
 * 
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/* TODO 할 일들
 * 파일 시그네이쳐 인식
 * 디렉토리 이미 존재할 경우 예외 처리
 * 압축된 경우 압축 비트 읽어서 압축 풀 것
 */

#include <gsf/gsf-utils.h>
#include <gsf/gsf-input-stdio.h>
#include <gsf/gsf-outfile-stdio.h>
#include <gsf/gsf-infile.h>
#include <gsf/gsf-infile-msole.h>
#include <gsf/gsf-structured-blob.h>

int main (int argc, char **argv)
{
    GError    *err;
    GsfInput  *input;
    GsfInfile *infile;

    gsf_init ();
    input  = gsf_input_stdio_new (argv[1], &err);
    infile = gsf_infile_msole_new (input, &err);

    /* hwp-dir 만들고 거기에 파일들을 풀어 넣는 소스 */
    GsfOutfile *folder;
    folder = gsf_outfile_stdio_new ("hwp-dir", &err);

    for (int i = 0; i < gsf_infile_num_children (infile); i++)
    {
        GsfInput *item = gsf_infile_child_by_index (infile, i);
        GsfStructuredBlob *itemfile = gsf_structured_blob_read (item);
        gsf_structured_blob_write (itemfile, folder);
    }

    gsf_shutdown ();
}
