function normal	(shader, t_base, t_second, t_detail)
	shader:begin	("wd_body", "wd_body")
		: fog		(false)
	shader:sampler("s_base"):texture(t_base):wrap():f_anisotropic()
	shader:sampler("s_wdnrm"):texture(t_base .. "_bump"):wrap():f_anisotropic()
end
