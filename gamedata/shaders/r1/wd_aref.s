function normal	(shader, t_base, t_second, t_detail)
	shader:begin	("wd_body", "wd_aref")
		: fog		(false)
	shader:sampler("s_base"):texture(t_base):wrap():f_anisotropic()
end
