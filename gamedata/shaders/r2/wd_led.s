function normal(shader, t_base, t_second, t_detail)
	shader:begin	("wd_nixie", "wd_led")
		: fog		(false)
		: zb		(true,false)
		: blend		(true,blend.one,blend.invsrcalpha)
		: sorting	(1, true)

	shader:sampler("s_base"):texture(t_base):wrap():f_anisotropic()
	shader:sampler("s_wdnrm"):texture(t_base .. "_bump"):wrap():f_anisotropic()
end
