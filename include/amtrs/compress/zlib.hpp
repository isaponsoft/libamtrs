// zlib(Deflate)





template<class IN, size_t INBUFFSIZE = 8192, size_t OBUFFSIZE = 8192>
void inflate(IN _in)
{
				char		ibuff[8192];
				char		obuff[8192];
				int	flush	= Z_SYNC_FLUSH;


				z_stream	zin;
				std::memset(&zin, 0, sizeof(zin));
				zin.zalloc		= nullptr;
				zin.zfree 		= nullptr;
				zin.opaque		= nullptr;
				inflateInit2(&zin, -MAX_WBITS);

				auto		insize	= lfh.compressSize;
				file_seek(in, beforeZip + filepos, std::ios::beg);

				zin.next_out	= (Bytef*)obuff;
				zin.avail_out	= sizeof(obuff);
				for (;;)
				{
					// 入力バッファが空っぽになった
					if (zin.avail_in == 0)
					{
						zin.next_in		= (Bytef*)ibuff;
						auto	reads	= std::min<size_t>(insize, sizeof(ibuff));
						zin.avail_in	= io::read((void*)ibuff, in, reads);
						if (zin.avail_in < sizeof(ibuff))
						{
							flush	= Z_FINISH;
						}
						insize	-= reads;
					}
					// 出力バッファを使い切った
					if (zin.avail_out == 0)
					{
						o.write(obuff, sizeof(obuff));
						zin.next_out	= (Bytef*)obuff;
						zin.avail_out	= sizeof(obuff);
					}

					auto r = inflate(&zin, flush);
					if (r == Z_STREAM_END)
					{
						// complite
						break;
					}
					switch (r)
					{
						case Z_OK:
						case Z_BUF_ERROR :
							break;

						case Z_NEED_DICT:    // 辞書が必要
							AMTRS_TRACE_LOG("Z_NEED_DICT");
							return;
						case Z_DATA_ERROR:   // 不正なデータ
							AMTRS_TRACE_LOG("Z_DATA_ERROR");
							return;

						case Z_STREAM_ERROR:
							AMTRS_TRACE_LOG("Z_STREAM_ERROR");
							return;
					}
				}
				o.write(obuff, sizeof(obuff) - zin.avail_out);

				inflateEnd(&zin);

}
