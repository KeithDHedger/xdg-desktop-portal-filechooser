/*
 *
 * ©K. D. Hedger. Wed 20 Nov 14:37:38 GMT 2024 keithdhedger@gmail.com

 * This file (filechooser.vala) is part of xdg-desktop-portal-filechooser.

 * xdg-desktop-portal-filechooser is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * xdg-desktop-portal-filechooser is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with xdg-desktop-portal-filechooser.  If not, see <http://www.gnu.org/licenses/>.

   Based on original code here:
   https://github.com/capezotte/xdg-desktop-portal-scriptfm
*/

string to_one(bool x)
{
	return x ? "1" : "";
}

const uint OK=0;
const uint FAIL=2;
const uint ENDED=3;

[DBus(name="org.freedesktop.impl.portal.Request")]
public class ScriptRequest : Object
{
	private		weak DBusConnection conn;
	protected	Cancellable cancellable;
	private		uint id;
	private		string[] argv;

	public ScriptRequest(string[] argv,ObjectPath h,DBusConnection conn) throws IOError
		{
			this.conn=conn;
			this.id=this.conn.register_object(h,this);
			this.cancellable=new Cancellable();
			this.argv=argv;

			this.cancellable.connect(() =>
			{
				this.conn.unregister_object(this.id);
			});
		}

	protected async Bytes? run() throws Error
		{
			Bytes ret=new Bytes(null);
			bool comm=false;
			Subprocess proc;

			try
				{
					proc=new Subprocess.newv(this.argv,SubprocessFlags.STDOUT_PIPE);
					this.cancellable.connect(() =>
						{
							proc.send_signal(ProcessSignal.HUP);
						});
				}
			catch(Error e)
				{
					stderr.printf("failed to create script process: %s\n",e.message);
					return null;
				}

			proc.communicate_async.begin(null,this.cancellable,(obj,res) =>
				{
					try
						{
							comm=proc.communicate_async.end(res,out ret,null);
						}
					catch(Error e)
						{
							stderr.printf("script communication failed: %s\n",e.message);
							comm=false;
						}
					finally
						{
							this.conn.unregister_object(this.id);
						}
					Idle.add(this.run.callback);
				});

			yield;

			if(!comm)
				{
					stderr.printf("giving up: script communication failed...\n");
					return null;
				}
			else if(!proc.get_successful())
				{
					stderr.printf("failed: child exited %d\n",proc.get_if_signaled() ? 127 + proc.get_term_sig() : proc.get_exit_status());
					return null;
				}

			stderr.printf("child successful!\n");
			return ret;
		}

	public void close() throws DBusError,IOError
		{
			this.cancellable.cancel();
		}
}

public class FileRequest : ScriptRequest
{
	private bool multiple;

	public FileRequest(bool save,HashTable<string,Variant> opts,ObjectPath h,DBusConnection conn) throws Error
		{
		/* BUG: this.something segfaults until after base call */
			bool		multiple=false;
			bool		directory=false;
			string	path=null;
			string	filename="";
			string	envfilter="";

			opts.for_each((k,v) =>
				{
					//stderr.printf("key name=>%s<\n",k);
					if(k=="current_file")
						{
							string	fn=(string)v.get_bytestring();
							int		cnt=fn.last_index_of("/")+1;
							fn=fn.substring(cnt);
							filename =fn;
						}
					else if(k=="current_name")
						{
							filename=(string)v.get_string();
						//	stderr.printf("filename=-->>>%s<<<<--\n",filename);
						}
					else if(k=="directory")
						{
							directory=v.get_boolean();
						}
					else if(k=="multiple")
						{
							multiple=v.get_boolean();
						}
					else if(k=="current_folder")
						{
							path=(string)v.get_bytestring();
							//stderr.printf("filename=-->>>%s<<<<--\n",path);
						}
					else if(k=="filters")
						{
							string		key=null;
							string		ls=null;
							Variant		vrc=null;
							Variant		vrcc=null;
							Variant		vr=null;
							int			y=0;
							VariantIter	iter=v.iterator();

							while(iter.next("(sa(us))",&key))
								{
									//stderr.printf("-->>>%s<<<<--\n",key);
									envfilter=envfilter.concat(key,"( ",null);
									vr=v.get_child_value(y++);
									//stderr.printf("%s\n",vr.print(true));
									vrc=vr.get_child_value(1);
									for(int q=0;q<(int)vrc.n_children();q++)
										{
											vrcc=vrc.get_child_value(q);
											VariantIter iter3=vrcc.iterator();
											for(int i=0;i<iter3.n_children();i=i+2)
												{
													int	dump=0;
													iter3.next("u",&dump);
													iter3.next("s",&ls);
													//stderr.printf("-->>>%s<<<<--\n",ls);
													envfilter=envfilter.concat(" ",ls,null);
												}
										}
									envfilter=envfilter.concat(" )|",null);
										
								}
//stderr.printf("enffilter= %s\n",envfilter);
						}
					else
						{
							stderr.printf("ignoring key: %s\n",k);
						}
				});

			string[] args =
				{
					"env",
					"SFM_MULTIPLE=" + to_one(multiple),
					"SFM_DIRECTORY=" + to_one(directory),
					"SFM_SAVE=" + to_one(save),
					"SFM_PATH=" +(path ?? ""),
					"SFM_NAME=" +(filename ?? ""),
					"SFM_FILT=" + envfilter,
					Environment.get_variable("SFM_FILE_SCRIPT") ?? "xdp-sfm",
				};

			base(args,h,conn);
			this.multiple=multiple;
		}

	public new async void run(out uint rep,out HashTable<string,Variant> results) throws Error
		{
			results=new HashTable<string,Variant>(str_hash,str_equal);
			Bytes? data_raw=yield base.run();

			if(data_raw==null || data_raw.length <= 1)
				{
					rep=FAIL;
					return;
				}

			/* ensure NUL termination */
			var data=Bytes.unref_to_array(data_raw);
			if(data.data[data.data.length-1] != 0)
				{
					data.append({0});
				}

			Array<string> filenames=new Array<string>();
			uint8[]? content=data.data;
			size_t i=0;

			for(size_t j=0; j < content.length; j++)
				{
					if(content[j]==0)
						{
							string chosen=(string)content[i:];
							try
								{
									if(filenames.length==0 || this.multiple)
										{
											filenames.append_val(Filename.to_uri(chosen));
										}
								}
							catch(ConvertError e)
								{
									stderr.printf("invalid file %s(%s)\n",chosen,e.message);
								}
							i=j + 1;
						}
				}

			rep=OK;
			results.insert("uris",filenames.data);
		}
}

[DBus(name="org.freedesktop.impl.portal.FileChooser")]
public class ScriptFileManager : Object
{

	private DBusConnection conn;

	public ScriptFileManager(DBusConnection conn)
		{
			this.conn=conn;
		}

	private async void open_save(bool save,ObjectPath handle,HashTable<string,Variant> options,out uint response,out HashTable<string,Variant> results) throws DBusError,IOError
		{
			/* begin */
			FileRequest req;
			try
				{
					req=new FileRequest(save,options,handle,this.conn);
				}
			catch(Error e)
				{
					stderr.printf("Failed to construct file request: %s\n",e.message);
					response=FAIL;
					results=new HashTable<string,Variant>(str_hash,str_equal);
					return;
				}

			AsyncResult? outer_res=null;
			req.run.begin((obj,res) =>
				{
					outer_res=res;
					Idle.add(open_save.callback);
				});

			yield;

			if(outer_res != null)
				{
					try
						{
							req.run.end(outer_res,out response,out results);
						}
					catch(Error e)
						{
							stderr.printf("request for file failed: %s\n",e.message);
							response=FAIL;
						}
				}
			else
				{
					response=ENDED;
					results=new HashTable<string,Variant>(str_hash,str_equal);
				}
		}

	public async void open_file(ObjectPath h,string app_id,string parent,string title,HashTable<string,Variant> o,out uint rep,out HashTable<string,Variant> res) throws DBusError,IOError
		{
			yield open_save(false,h,o,out rep,out res);
		}

	public async void save_file(ObjectPath h,string app_id,string parent,string title,HashTable<string,Variant> o,out uint rep,out HashTable<string,Variant> res) throws DBusError,IOError
		{
			yield open_save(true,h,o,out rep,out res);
		}
}

void on_bus_acquired(DBusConnection conn)
{
	try
		{
			var service=new ScriptFileManager(conn);
			conn.register_object("/org/freedesktop/portal/desktop",service);
		}
	catch(IOError e)
		{
			stderr.printf("failed to hop on the Desktop Bus: %s\n",e.message);
		}
}

void main()
{
	Bus.own_name(BusType.SESSION,"org.freedesktop.impl.portal.desktop.filechooser",BusNameOwnerFlags.NONE,on_bus_acquired,/* callback function on registration succeeded */() => {},/* callback on name register succeeded */() => stderr.printf("Could not acquire name\n"));

	new MainLoop().run();
}
